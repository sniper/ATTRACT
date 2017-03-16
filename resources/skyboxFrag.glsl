#version 330 core

uniform samplerCube cubemap;

in vec3 vNor; // normal in world space

out vec4 color;

uniform float T;


void main()
{
    // We want the normal to be in world coords, not camera coords.
    color = texture(cubemap, vNor);




    
}
