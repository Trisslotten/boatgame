#version 440 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;

uniform mat4 viewProj;

out vec3 v_pos;

void main()
{
	
	gl_Position = viewProj * vec4(pos, 1.0);
	v_pos = gl_Position.xyz;

}
