#version 330 core

uniform sampler2D shadowDepth;

uniform float lightIntensity;
uniform vec3 lightPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

in vec4 fragPosInCam; // passed from vert shader
in vec4 fragNorInCam; // passed from vert shader
in vec4 fragPosInLight;
in vec3 vColor;

out vec4 color;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {
    float bias = 0.1;
    
    //1: shift the coordinates from -1, 1 to 0,1
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
    vec3 n = normalize(fragNorInCam.xyz);
    vec3 e = -normalize(fragPosInCam.xyz);
    
    vec3 l = normalize(lightPos - fragPosInCam.xyz);
    vec3 h = normalize(e + l);
    vec3 cd = kd * max(0, dot(l, n));
    vec3 cs = ks * pow(max(0, dot(h, n)), s);
    
    float Shade;
    float amb = 0.3;
    vec4 BaseColor = vec4(vColor, 1);
    
    Shade = TestShadow(fragPosInLight);
    
    vec4 actualColor = vec4(lightIntensity * (ka + cd + cs), 1.0);
    
    color = amb*actualColor + (1.0-Shade)*actualColor;
}
