#version 330 core
uniform sampler2D myTextureSampler;
in vec4 partCol;
out vec4 Outcolor;

void main()
{
	float alpha = texture(myTextureSampler, gl_PointCoord).r;
	Outcolor = vec4(partCol.rgb, alpha);
}
