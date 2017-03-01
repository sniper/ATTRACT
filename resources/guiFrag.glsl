#version 330 core

uniform sampler2D guiTex;

varying vec2 vTex;

out vec4 color;

void main(){
  color = texture2D(guiTex, vTex);
  //color = vec4(1,0.1,0.5,0.5);
}
