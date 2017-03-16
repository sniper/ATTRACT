#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 vTex;
out vec3 fragPos;
out vec3 fragNor;
out mat3 TBN;

void main()
{
    gl_Position = P * V * M * aPos;
    vTex = aTex;

    vec3 T = normalize(vec3(V * M * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(V * M * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(V * M * vec4(aNor, 0.0)));
    TBN = mat3(T, B, N);
    
    /* frag position in world */
    fragPos = vec3(M * aPos);
    /* frag normal in world */
    fragNor = vec3(M * vec4(aNor, 0.0));
    /* frag position in view */
    fragPos = vec3(V * M * aPos);
}
