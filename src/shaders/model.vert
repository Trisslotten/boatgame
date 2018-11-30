#version 440 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;

out vec3 vnormal;


uniform mat4 viewProj;

void main()
{
	vnormal = normal;
	gl_Position = viewProj * vec4(pos, 1.0);
}
