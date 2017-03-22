#version 330 core

uniform samplerCube cubemap;

in vec3 vNor; // normal in world space
in vec3 pos;
out vec4 color;

uniform float T;


void main()
{
    // We want the normal to be in world coords, not camera coords.

        
        color = texture(cubemap, pos);




    
}
