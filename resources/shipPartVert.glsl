#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat4 objTransMatrix;

attribute vec4 aPos;
attribute vec3 aNor;
attribute vec2 aTex;

varying vec2 vTex;
varying vec4 fragPosInCam;
varying vec4 fragNorInCam;

void main()
{
    gl_Position = P * MV * objTransMatrix * aPos;
    vTex = aTex;
    fragPosInCam = MV * objTransMatrix * aPos;
    fragNorInCam = MV * objTransMatrix * vec4(aNor, 0);
}
