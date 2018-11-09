#version 440 core

layout(vertices = 1) out;
in vec2 vPatchPos[];

patch out vec2 tcPatchPos;
patch out float tcLevel;

uniform float numPatches;
uniform vec3 size;

uniform mat4 viewProj;
uniform vec3 cameraPos;
uniform vec2 windowSize;
uniform float fov;

const float pixelsPerQuad = 8.0;


float uhash12(uvec2 x)
{
	uvec2 q = 1103515245U * ((x >> 1U) ^ (uvec2(x.y, x.x)));
	uint  n = 1103515245U * ((q.x) ^ (q.y >> 3U));
	return float(n) * (1.0 / float(0xffffffffU));
}
float hash12(vec2 x) { return uhash12(uvec2(50.*x)); }


// determine the tessellation level
float level(vec2 offset) 
{
	float patchSize = size.x/numPatches;
	vec2 pos = vPatchPos[gl_InvocationID] + 0.5*patchSize + patchSize * offset;
	float dist = length(vec3(pos.x, 0, pos.y) - cameraPos);
	float b = dist*tan(fov*0.5);
	float ratio = patchSize/(2.0*b);
	float pixels = windowSize.x * ratio;
	float res = pixels/pixelsPerQuad;
	// dither for better level transition
	//res *= 1.0 + 0.2*hash12(pos);
	//return max(pow(2.f, ceil(log(res)/log(2))), 1.f);
	return res;
}

void main() 
{		
	float currLevel = level(vec2(0));

	gl_TessLevelOuter[0] = currLevel;
	gl_TessLevelOuter[1] = currLevel;
	gl_TessLevelOuter[2] = currLevel;
	gl_TessLevelOuter[3] = currLevel;

	tcLevel = currLevel;

	// check level of bordering quads to avoid holes in mesh
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