#version 130 

attribute vec3 pos;

void main(){
  gl_Position.xyz = pos;
  gl_Position.w = 1;
}