#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 posTex;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

out vec2 vTex;

void main(){
  vec4 temp = vec4(pos.x, pos.y, pos.z, 1);
  gl_Position = P * V * M * temp;
  gl_Position.w = 1;
  gl_Position.y = -gl_Position.y;
  gl_Position.x = -gl_Position.x;
  vTex = posTex;
  vTex.y = vTex.y * -1;
}
