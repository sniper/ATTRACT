#version 330 core

const int NUM_CASCADES = 3;

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;
uniform sampler2D shadowDepth0;
uniform sampler2D shadowDepth1;
uniform sampler2D shadowDepth2;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 cascadeEndClipSpace;

in vec2 vTex;
in vec3 fragPos;
in vec3 fragNor;
in mat3 TBN;
in vec4 fragPosLS[NUM_CASCADES];
in float clipSpacePosZ;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(int cascadeIndex, vec4 LSfPos) {
    float bias = 0.0012;
    
    // perspective divide isn't needed since we're using ortho projection, but
    // doesn't hurt.
    vec3 projCoords = LSfPos.xyz / LSfPos.w;
    
    //1: shift the coordinates from -1, 1 to 0 ,1
    vec3 shift = projCoords * 0.5 + 0.5;
    //2: read off the stored depth (.r) from the ShadowDepth, using the shifted.xy
    float curD = shift.z;
    float lightD;
    if (cascadeIndex == 0) {
        lightD = texture(shadowDepth0, shift.xy).r;
    }
    else if (cascadeIndex == 1) {
        lightD = texture(shadowDepth1, shift.xy).r;
    }
    else if (cascadeIndex == 2) {
        lightD = texture(shadowDepth2, shift.xy).r;
    }
    
    if (shift.x > 1.0 || shift.y > 1.0 || shift.x < 0.0 || shift.y < 0.0) {
        return -1.0;
    }
    
    //3: compare to the current depth (.z) of the projected depth
    //4: return 1 if the point is shadowed
    if (curD - bias > lightD) {
        return 1.0;
    }
    
    return 0.0;
}

void main()
{
    vec3 color = texture(diffuseTex, vTex).rgb;
    //    vec3 ks = texture(specularTex, vTex).rgb;

    vec3 normal = texture(normalTex, vTex).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    vec3 lightColor = vec3(1.0);
    vec3 ambient = 0.3 * color;
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    float shadow;
    for (int i = 0; i < NUM_CASCADES; i++) {
        if (clipSpacePosZ <= cascadeEndClipSpace[i]) {
            if (i == 0) {
                shadow = TestShadow(0, fragPosLS[0]);
            }
            else if (i == 1) {
                shadow = TestShadow(1, fragPosLS[1]);
            }
            else if (i == 2) {
                shadow = TestShadow(2, fragPosLS[2]);
            }
            break;
        }
    }

    if (shadow < -0.5) {
        shadow = 0.0f;
    }
    
    vec3 lighting = ambient + ((1.0 - shadow) * (diffuse + specular)) * color;

    // add fog
    float dist = abs(distance(fragPos, viewPos));
    float f = 1 / exp(dist * 0.10);
    float e = 0.3;
    lighting = (lighting * (1 - e)) + (vec3(0.5, 0.5, 0.55) * e);

    FragColor = vec4(lighting, 1.0f);
    BloomColor = vec4(0.0);
}
