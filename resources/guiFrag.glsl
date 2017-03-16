#version 330 core

uniform sampler2D guiTex;

in vec2 vTex;

out vec4 color;

uniform float alpha;

void main(){
  color = texture(guiTex, vTex);
   if(alpha != -1)
    color.a = alpha;
}
