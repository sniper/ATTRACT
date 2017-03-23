#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec4 Pcolor;
uniform mat4 P;
uniform mat4 V;

uniform vec3 Color;

out vec4 partCol;

void main() { 

	// Billboarding: set the upper 3x3 to be the identity matrix
	mat4 V0 = V;
	V0[0] = vec4(1.0, 0.0, 0.0, 0.0);
	V0[1] = vec4(0.0, 1.0, 0.0, 0.0);
	V0[2] = vec4(0.0, 0.0, 1.0, 0.0);
	gl_Position = P * V* vec4(vertPos.xyz, 1.0);
        //gl_Position.z = gl_Position.z;
	partCol = vec4(Color, 0.5f);
        gl_PointSize = distance( gl_Position, vertPos) * 5.5 ;
}
