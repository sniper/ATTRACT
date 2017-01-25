#version 120

uniform sampler2D grassTexture;

varying vec2 vTex;

void main()
{
    gl_FragColor = texture2D(grassTexture, vTex);
}