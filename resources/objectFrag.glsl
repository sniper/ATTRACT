#version 120

uniform float lightIntensity;
uniform vec4 lightPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;
uniform int selected;

varying vec4 fragPosInCam; // passed from vert shader
varying vec4 fragNorInCam; // passed from vert shader

void main()
{
    vec3 n = normalize(fragNorInCam.xyz);
    vec3 e = -normalize(fragPosInCam.xyz);
    
    vec3 l = normalize(lightPos.xyz - fragPosInCam.xyz);
    vec3 h = normalize(e + l);
    vec3 cd = kd * max(0, dot(l, n));
    vec3 cs = ks * pow(max(0, dot(h, n)), s);

    if (selected == 0) {
        gl_FragColor = vec4(lightIntensity * (ka + cd + cs), 1.0);
    } else {
        gl_FragColor = vec4(lightIntensity * (vec3(0, 0, 0) + cd + cs), 1.0);
    }
}

