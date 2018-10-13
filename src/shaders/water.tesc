#version 440 core

layout(vertices = 1) out;
in vec2 vPatchPos[];

patch out vec2 tcPatchPos;


uniform float numPatches;
uniform vec3 size;


uniform mat4 viewProj;
uniform vec3 cameraPos;
uniform vec2 windowSize;
uniform float fov = 70.0;

const float pixelsPerQuad = 7.0;

float level(vec2 offset) 
{
	float patchSize = size.x/numPatches;
	vec2 pos = vPatchPos[gl_InvocationID] + 0.5*patchSize + patchSize * offset;
	//pos += size.xz * hmPos;
	float dist = length(vec3(pos.x, 0, pos.y) - cameraPos);
	float b = dist*tan(fov/2.0);
	float ratio = patchSize/(2.0*b);
	float pixels = windowSize.x * ratio;
	float res = pixels/pixelsPerQuad;
	//res = pow(res, 2.f);
	return max(pow(2.f, ceil(log(res)/log(2))), 1.f);
}

void main() 
{		
	float currLevel = level(vec2(0));

	gl_TessLevelOuter[0] = currLevel;
	gl_TessLevelOuter[1] = currLevel;
	gl_TessLevelOuter[2] = currLevel;
	gl_TessLevelOuter[3] = currLevel;

	float left = level(vec2(-1,0));
	if(left < currLevel) {
		gl_TessLevelOuter[0] = left;
	}
	float bottom = level(vec2(0,-1));
	if(bottom < currLevel) {
		gl_TessLevelOuter[1] = bottom;
	}
	float right = level(vec2(1,0));
	if(right < currLevel) {
		gl_TessLevelOuter[2] = right;
	}
	float top = level(vec2(0,1));
	if(top < currLevel) {
		gl_TessLevelOuter[3] = top;
	}
	
	gl_TessLevelInner[0] = currLevel;
	gl_TessLevelInner[1] = currLevel;

	tcPatchPos = vPatchPos[gl_InvocationID];
}