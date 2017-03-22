#version 330 core

uniform samplerCube cubemap;

in vec3 pos; // normal in world space

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

void main()
{
    // We want the normal to be in world coords, not camera coords.

    FragColor = texture(cubemap, pos);
    BloomColor = vec4(0.0);
}
