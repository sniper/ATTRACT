#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat4 objTransMatrix;

attribute vec4 aPos; // in object space
attribute vec3 aNor; // in object space

varying vec4 fragPosInCam; // aPos in camera space
varying vec4 fragNorInCam; // aNor in camera space

void main()
{
    gl_Position = P * MV * objTransMatrix * aPos;
    fragPosInCam = MV * objTransMatrix * aPos;
    vec4 n = vec4(aNor, 0);
    fragNorInCam = MV * objTransMatrix * n;
}