#version 330 core

uniform float lightIntensity;
uniform vec4 lightPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

in vec4 fragPosInCam; // passed from vert shader
in vec4 fragNorInCam; // passed from vert shader

out vec4 color;

void main()
{
    vec3 n = normalize(fragNorInCam.xyz);
    vec3 e = -normalize(fragPosInCam.xyz);
    
    vec3 l = normalize(lightPos.xyz - fragPosInCam.xyz);
    vec3 h = normalize(e + l);
    vec3 cd = kd * max(0, dot(l, n));
    vec3 cs = ks * pow(max(0, dot(h, n)), s);
    
    color = vec4(lightIntensity * (ka + cd + cs), 0.5);
}
