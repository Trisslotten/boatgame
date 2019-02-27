#version 440 core

out vec4 outColor;

in vec3 vnormal;

uniform vec3 sunDir;

void main()
{
	vec3 normal = normalize(vnormal);

	vec3 lighting = vec3(0);
	lighting += max(dot(sunDir, normal),0);
	lighting += 0.2;

	outColor = vec4(lighting,1);
}