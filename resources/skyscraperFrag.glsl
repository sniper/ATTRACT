#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;
uniform vec3 lightPos;

in vec2 vTex;
in vec4 fragPosInCam;
in vec4 fragNorInCam;
in mat3 TBN;

out vec4 color;

void main()
{
    vec3 kd = texture(diffuseTex, vTex).rgb;
    vec3 ks = texture(specularTex, vTex).rgb;
    
    //vec3 n = normalize(fragNorInCam.xyz);
    vec3 n = texture(normalTex, vTex).rgb;
    if (n.g != 1.0) { // pure green is marking the top/bottom for now
        n = normalize(n * 2.0 - 1.0);
        n = normalize(TBN * n);
    } else {
        n = normalize(fragNorInCam.xyz);
    }

    vec3 l = normalize(lightPos - fragPosInCam.xyz);
    vec3 h = normalize(normalize(-fragPosInCam.xyz) + l);
    
    vec3 cd = kd * max(0, dot(l, n));
    vec3 cs = ks * pow(max(0, dot(h, n)), 50.0f);
    
    color = vec4(cd + cs, 1.0);
}
