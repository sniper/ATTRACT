#version 330 core

uniform vec3 ka;

out vec4 FragColor;

void main()
{
    FragColor = vec4(ka, 1.0);
}
