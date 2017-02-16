#version 120 

uniform sampler2D guiTex;

varying vec2 vTex;

void main(){
  gl_FragColor = texture2D(guiTex, vTex);
}
