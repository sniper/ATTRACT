#version 330 core

layout(location = 0) in vec4 aPos; // in object space
layout(location = 1) in vec3 aNor; // in object space

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 LS;
uniform vec3 lightPos;

out vec4 fragPosInCam; // aPos in camera space
out vec4 fragNorInCam; // aNor in camera space
out vec4 fragPosInLight; // aPos in light space
out vec3 vColor;

void main()
{
    gl_Position = P * V * M * aPos;
    fragPosInCam = V * M * aPos;
    vec4 n = vec4(aNor, 0);
    fragNorInCam = V * M * n;
    fragPosInLight = LS * vec4(fragPosInCam);
    vColor = vec3(max(dot(fragNorInCam, normalize(vec4(lightPos, 1.0))), 0));
}
