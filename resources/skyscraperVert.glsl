#version 330 core

uniform mat4 P;
uniform mat4 MV;
uniform mat4 objTransMatrix;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec2 aTex;

out vec2 vTex;
out vec4 fragPosInCam;
out vec4 fragNorInCam;

void main()
{
    gl_Position = P * MV * objTransMatrix * aPos;
    vTex = aTex;
    fragPosInCam = MV * objTransMatrix * aPos;
    fragNorInCam = MV * objTransMatrix * vec4(aNor, 0);
}
