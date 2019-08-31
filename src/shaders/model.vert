#version 440 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;

out vec3 vnormal;


uniform float time;
uniform mat4 viewProj;
uniform vec3 position;

void main()
{
	//float s = sin(time);
	//float c = cos(time);
	//mat3 rot = mat3(vec3(s, c, 0), vec3(-c, s, 0), vec3(0, 0, 1));

	vnormal = normalize(normal);
	gl_Position = viewProj * vec4(pos + position, 1.0);
}
