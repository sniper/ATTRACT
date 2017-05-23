#version 330 core

layout(location = 0) in vec4 aPos; // in object space
layout(location = 1) in vec3 aNor; // in object space

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 LS;

out vec3 fragPos;
out vec3 fragNor;
out vec4 fragPosLS;

void main()
{
    gl_Position = P * V * M * aPos;
    /* frag position in world */
    fragPos = vec3(M * aPos);
    /* frag normal in world */
    fragNor = vec3(M * vec4(aNor, 0.0));
    /* frag pos in light space */
    fragPosLS = LS * vec4(fragPos, 1.0);
}
