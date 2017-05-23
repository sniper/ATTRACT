#version 330 core

uniform sampler2D shadowDepth;

uniform float lightIntensity;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int time;

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
    float lightD = texture(shadowDepth, shift.xy).r;
    
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

float rand(vec2 co, int seed){
    return fract(sin(dot(co * seed, vec2(12.9898,78.233))) * 43758.5453);
    //return fract(sin(dot(co * (seed % 30), vec2(12.9898,78.233))) * 43758.5453);
    //return fract(sin(dot(vec2(co.y, co.x) * seed, vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    //float color = rand(vec2(fragPos), time);
    //if (color > 0.7 || color < 0.3) {
    //    color = 0.5;
    //}

    //float shadow = TestShadow(fragPosLS);
    //if (shadow < -0.5) {
    //    color -= 0.3;
    //}

    //FragColor = vec4(vec3(color / 2, color, color / 2), (dist / 30) + color);
    //FragColor = vec4(vec3(color, color, color), (dist / 30) + color);


    float dist = abs(distance(fragPos, viewPos));
    // make sure there's always a little bit of fog
    if (dist < 5.0) {
        dist = 5.0;
    }
    vec3 fogColor = vec3(0.5, 0.5, 0.55);
    float f = 1 / exp(dist * 0.10);
    // make sure there's always a little opacity
    if (1 - f > 0.9) {
        f = 0.1;
    }

    FragColor = vec4(fogColor, 1 - f);
}
