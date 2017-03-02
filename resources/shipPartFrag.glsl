#version 330 core

uniform sampler2D colorTexture;
uniform sampler2D specularTexture;
uniform vec3 lightPos;

in vec2 vTex;
in vec4 fragPosInCam;
in vec4 fragNorInCam;

out vec4 color;

void main()
{
    vec3 kd = texture(colorTexture, vTex).rgb;
    vec3 ks = texture(specularTexture, vTex).rgb;
    
    vec3 n = normalize(fragNorInCam.xyz);
    vec3 l = normalize(lightPos - fragPosInCam.xyz);
    vec3 h = normalize(normalize(-fragPosInCam.xyz) + l);
    
    vec3 cd = kd * max(0, dot(l, n));
    vec3 cs = ks * pow(max(0, dot(h, n)), 50.0f);
    
    color = vec4(cd + cs, 1.0);
}
