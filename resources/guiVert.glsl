#version 130 

attribute vec3 pos;
attribute vec2 posTex;

varying vec2 vTex;

void main(){
  gl_Position.xyz = pos;
  gl_Position.w = 1;


  vTex = posTex;
  vTex.y = vTex.y * -1;
}