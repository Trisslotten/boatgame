#version 440 core

out vec4 outColor;

in vec3 tepos;

uniform vec3 sunDir;

void main() 
{
	vec3 normal = normalize(cross(dFdx(tepos), dFdy(tepos)));

	vec3 color = vec3(1);

	vec3 lighting = vec3(0);
	lighting += color * clamp(dot(normal, sunDir), 0.0, 1.0);
	lighting += color * 0.1;

	outColor = vec4(lighting, 1);
}