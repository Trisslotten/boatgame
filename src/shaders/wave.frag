#version 440 core

layout (location = 0) out vec4 outDisp;

in vec2 tex;

uniform float time;

void main()
{
	outDisp = vec4(0);
	outDisp.y = sin(tex.x*10+time*0.5) * sin(tex.y*10 + time*0.3);
	outDisp.a = 1.0;
}