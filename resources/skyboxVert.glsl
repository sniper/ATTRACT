#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNor;

out vec3 vNor; // normal in world space

void main()
{
    vNor = aNor;
    gl_Position = P * V * M * aPos;
}
