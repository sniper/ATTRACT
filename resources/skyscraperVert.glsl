#version 330 core

uniform mat4 P;
uniform mat4 MV;
uniform mat4 objTransMatrix;
uniform vec3 scalingFactor;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNor;
layout(location = 2) in vec2 aTex;

out vec2 vTex;
out vec4 fragPosInCam;
out vec4 fragNorInCam;

float epsilon = 0.00001;

void main()
{
    gl_Position = P * MV * objTransMatrix * aPos;
    if (aNor.x > epsilon || aNor.x < -epsilon) {
        vTex = vec2(aTex.x * scalingFactor.z, aTex.y * scalingFactor.y);
    }
    else if (aNor.y > epsilon || aNor.y < -epsilon) {
        vTex = vec2(aTex.x * scalingFactor.x, aTex.y * scalingFactor.z);
    }
    else {
        vTex = vec2(aTex.x * scalingFactor.x, aTex.y * scalingFactor.y);
    }
    
    fragPosInCam = MV * objTransMatrix * aPos;
    fragNorInCam = MV * objTransMatrix * vec4(aNor, 0);
}
