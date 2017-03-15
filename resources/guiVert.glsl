#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 posTex;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform float F;

out vec2 vTex;

void main(){
  vec4 temp = vec4(pos.x, pos.y, pos.z, 1);
    if(F == 0)
       temp = temp + vec4(1,0,0,0) + vec4(0,1,0,0);
  gl_Position = P * V * M * temp;
  if(F == 1)
    gl_Position.w = 1;
  gl_Position.y = -gl_Position.y;
  gl_Position.x = -gl_Position.x;
  vTex = posTex;
  vTex.y = vTex.y * -1;
}
