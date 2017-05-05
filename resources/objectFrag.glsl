#version 330 core

uniform sampler2D shadowDepth0;

uniform float lightIntensity;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

in vec3 fragPos; // passed from vert shader
in vec3 fragNor; // passed from vert shader
in vec4 fragPosLS;

out vec4 FragColor;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {
    float bias = 0.02;
    
    // perspective divide isn't needed since we're using ortho projection, but
    // just in case it changes to persepective.
    vec3 projCoords = LSfPos.xyz / LSfPos.w;
    
    //1: shift the coordinates from -1, 1 to 0 ,1
    vec3 shift = projCoords * 0.5 + 0.5;
    //2: read off the stored depth (.r) from the ShadowDepth, using the shifted.xy
    float curD = shift.z;
    float lightD = texture(shadowDepth0, shift.xy).r;
    
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
    vec3 n = normalize(fragNor);
    vec3 e = -normalize(fragPos);
    
    vec3 l = normalize(lightPos - fragPos);
    vec3 h = normalize(e + l);
    vec3 cd = kd * max(0, dot(l, n));
    vec3 cs = ks * pow(max(0, dot(h, n)), s);
    
    FragColor = vec4(lightIntensity * (ka + cd + cs), 0.5);
}
