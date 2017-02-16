#version 120

uniform sampler2D colorTexture;
uniform sampler2D specularTexture;
uniform vec3 lightPos;

varying vec2 vTex;
varying vec4 fragPosInCam;
varying vec4 fragNorInCam;

void main()
{
    vec3 kd = texture2D(colorTexture, vTex).rgb;
    vec3 ks = texture2D(specularTexture, vTex).rgb;
    
    vec3 n = normalize(fragNorInCam.xyz);
    vec3 l = normalize(lightPos - fragPosInCam.xyz);
    vec3 h = normalize(normalize(-fragPosInCam.xyz) + l);
    
    vec3 cd = kd * max(0, dot(l, n));
    vec3 cs = ks * pow(max(0, dot(h, n)), 50.0f);
    
    gl_FragColor = vec4(cd + cs, 1.0);
}
