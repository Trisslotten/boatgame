#version 440 core

layout (location = 0) in vec3 position;

uniform mat4 viewProj;

out vec3 texCoord;

void main() 
{
	texCoord = position;
	vec4 projPos = viewProj * vec4(position, 0.0);
	gl_Position = projPos.xyww;
}