#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec4 Pcolor;
uniform mat4 P;
uniform mat4 V;

out vec4 partCol;

void main() { 

	// Billboarding: set the upper 3x3 to be the identity matrix
	mat4 V0 = V;
	V0[0] = vec4(1.0, 0.0, 0.0, 0.0);
	V0[1] = vec4(0.0, 1.0, 0.0, 0.0);
	V0[2] = vec4(0.0, 0.0, 1.0, 0.0);
	gl_Position = P * V* vec4(vertPos, 1.0);
	partCol = Pcolor;
        //gl_PointSize = 14;
}
