#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 LS0;
uniform mat4 LS1;
uniform mat4 LS2;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec2 aTex;

const int NUM_CASCADES = 3;

out vec2 vTex;
out vec3 fragPos;
out vec3 fragNor;
out vec4 fragPosLS[NUM_CASCADES];
out float clipSpacePosZ;

void main()
{
    gl_Position = P * V * M * aPos;
    vTex = aTex;
    /* frag position in world */
    fragPos = vec3(M * aPos);
    /* frag normal in world */
    fragNor = vec3(M * vec4(aNor, 0.0));
    /* frag pos in light space */
    fragPosLS[0] = LS0 * vec4(fragPos, 1.0);
    fragPosLS[1] = LS1 * vec4(fragPos, 1.0);
    fragPosLS[2] = LS2 * vec4(fragPos, 1.0);
    clipSpacePosZ = gl_Position.z;
}
