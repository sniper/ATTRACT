#version 330 core

uniform samplerCube cubemap;

in vec3 vNor; // normal in world space

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

uniform float T;

void main()
{
    // We want the normal to be in world coords, not camera coords.
    FragColor = texture(cubemap, vNor);
    BloomColor = vec4(0.0);
}
