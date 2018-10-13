#version 440 core

layout(location = 0) in vec3 pos;

uniform mat4 viewProj;

void main()
{
	gl_PointSize = 5.0;
	gl_Position = viewProj * vec4(pos, 1.0);
}