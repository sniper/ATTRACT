#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 scalingFactor;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 vTex;
out vec4 fragPosInCam;
out vec4 fragNorInCam;
out mat3 TBN;

float epsilon = 0.00001;

void main()
{
    gl_Position = P * V * M * aPos;
    if (aNor.x > epsilon || aNor.x < -epsilon) {
        vTex = vec2(aTex.x * scalingFactor.z, aTex.y * scalingFactor.y)/5;
    }
    else if (aNor.y > epsilon || aNor.y < -epsilon) {
        vTex = vec2(aTex.x * scalingFactor.x, aTex.y * scalingFactor.z)/5;
    }
    else {
        vTex = vec2(aTex.x * scalingFactor.x, aTex.y * scalingFactor.y)/5;
    }
    
    fragPosInCam = V * M * aPos;
    fragNorInCam = V * M * vec4(aNor, 0);

    vec3 T = normalize(vec3(V * M * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(V * M * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(V * M * vec4(aNor, 0.0)));
    TBN = mat3(T, B, N);
}
