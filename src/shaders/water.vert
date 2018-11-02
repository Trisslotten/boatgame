#version 440 core

layout(location = 0) in vec2 pos;

out vec2 vPatchPos;

uniform vec3 cameraPos;
uniform float numPatches;
uniform vec3 cameraDir;
uniform vec3 size;

void main()
{
	vec2 spp = size.xz/numPatches;
	vec2 camOffset = cameraPos.xz + 0.49*(cameraDir * size.x).xz;
	camOffset= spp*round(camOffset/spp);
	vPatchPos = size.xz * pos / numPatches - 0.5*size.xz + camOffset;
}