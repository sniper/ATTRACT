#version  330 core

layout(location = 0) in vec3 aPos;

uniform mat4 LS;
uniform mat4 M;

void main() {
    
    /* transform into light space */
    gl_Position = LS * M * vec4(aPos.xyz, 1.0);
    
}
