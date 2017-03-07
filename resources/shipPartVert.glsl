#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 LS;
uniform vec3 lightPos;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec2 aTex;

out vec2 vTex;
out vec4 fragPosInCam;
out vec4 fragNorInCam;
out vec4 fragPosInLight;

void main()
{
    gl_Position = P * V * M * aPos;
    vTex = aTex;
    fragPosInCam = V * M * aPos;
    fragNorInCam = V * M * vec4(aNor, 0);
    fragPosInLight = LS * vec4(fragPosInCam);
}
