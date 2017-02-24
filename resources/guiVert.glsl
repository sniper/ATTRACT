#version 120 

attribute vec3 pos;
attribute vec2 posTex;

uniform mat4 M;

varying vec2 vTex;

void main(){
  vec4 temp = vec4(pos.x, pos.y, pos.z, 1);
  gl_Position = M * temp;
  gl_Position.w = 1;


  vTex = posTex;
  vTex.y = vTex.y * -1;
}