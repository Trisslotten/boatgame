#version 440 core

layout(vertices = 3) out;
in vec3 vpos[];
out vec3 tcpos[];

void main() 
{		
	float level = 64;

	gl_TessLevelOuter[0] = level;
	gl_TessLevelOuter[1] = level;
	gl_TessLevelOuter[2] = level;
	gl_TessLevelOuter[3] = level;

	gl_TessLevelInner[0] = level;
	gl_TessLevelInner[1] = level;

	tcpos[gl_InvocationID] = vpos[gl_InvocationID];
}
