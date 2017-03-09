#version 330 core

uniform sampler2D shadowDepth;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform vec4 lightPos;

in vec2 vTex;
in vec4 fragPosInCam;
in vec4 fragNorInCam;
in vec4 fragPosInLight;

out vec4 color;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {
    float bias = 0.1;
    
    //1: shift the coordinates from -1, 1 to 0 ,1
    vec3 shift = (LSfPos.xyz + vec3(1.0))/2.0;
    //2: read off the stored depth (.r) from the ShadowDepth, using the shifted.xy
    float curD = shift.z;
    float lightD = texture(shadowDepth, shift.xy).r;
    //3: compare to the current depth (.z) of the projected depth
    //4: return 1 if the point is shadowed
    if (curD - bias > lightD) {
        return 1.0;
    }
    
    return 0.0;
}

void main()
{
    vec3 kd = texture(diffuseTex, vTex).rgb;
    vec3 ks = texture(specularTex, vTex).rgb;
    
    vec3 n = normalize(fragNorInCam.xyz);
    vec3 l = normalize(lightPos.xyz - fragPosInCam.xyz);
    vec3 h = normalize(normalize(-fragPosInCam.xyz) + l);
    
    vec3 ca = kd * 0.2;
    vec3 cd = kd * max(0, dot(l, n));
    vec3 cs = ks * pow(max(0, dot(h, n)), 50.0f);
    color = vec4(ca + cd + cs, 1.0);
}
