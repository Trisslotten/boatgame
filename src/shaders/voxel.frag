#version 440 core

out vec4 outColor;


flat in int gID;

float uhash11(uint n)
{
	n = (n << 13U) ^ n;
	n = n * (n * n * 15731U + 789221U) + 1376312589U;
	return float(n & 0x7fffffffU) / float(0x7fffffff);
}

void main()
{
	vec3 color;
	color.r = uhash11(gID);
	color.g = uhash11(gID+100);
	color.b = uhash11(gID+200);
	outColor = vec4(color,1);
}