#version 440 core

layout(binding = 1, r8i) writeonly uniform iimage3D img;

in vec3 g_pos;

void main()
{
	ivec3 size = imageSize(img);
	ivec3 i = ivec3((0.5*g_pos+0.5) * vec3(size.x));
	imageStore(img, i, ivec4(1));
}