#version  330 core
layout(location = 0) in vec3 aPos;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(aPos, 1);
    texCoord = (aPos.xy+vec2(1, 1))/2.0;
}
