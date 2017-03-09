#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 LS;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec2 aTex;

out vec2 vTex;
out vec3 fragPos;
out vec3 fragNor;
out vec4 fragPosLS;

void main()
{
    gl_Position = P * V * M * aPos;
    vTex = aTex;
    /* frag position in world */
    fragPos = vec3(M * aPos);
    /* frag normal in world */
    fragNor = transpose(inverse(mat3(M))) * aNor;
    /* frag pos in light space */
    fragPosLS = LS * vec4(fragPos, 1.0);
}
