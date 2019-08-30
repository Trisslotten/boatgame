#version 440 core

layout(quads, fractional_even_spacing, cw) in;

patch in vec2 tcPatchPos;
patch in float tcLevel;

// tangent space
out vec3 tePosition;
out vec3 teCameraPos;
out vec3 teSunDir;
out vec3 teUp;
out float teAzimuth;
out float teZenith;

// world space
out vec3 teWPosition;
out vec3 teNonDisplaceWorld;

out float dist;
out mat3 teTBN;

uniform vec3 cameraPos;
uniform float numPatches;
uniform vec3 size;
uniform float time;
uniform mat4 viewProj;
uniform vec3 sunDir;
uniform float waterScale;

uniform sampler2D dispTex;

vec3 displace(vec3 pos)
{
	vec3 result = pos;
/*

	float look = length(cameraPos - pos);

	int len = waveData.length();
	for(int i = len-1; i >= 0 ; i--)
	{
		WaveData d = waveData[i];
		// to remove artifacts
		float weight = pow(smoothstep(6000, 0, look), 200.0/(i+1.0));
		if(weight < 0.0001)
			break;
		result += weight * gerstner(pos.xz, d);
	}
	//result *= pow(smoothstep(7000, 0, look), 10);

*/
	vec3 s;
	s.xz = vec2(1.0);
	s.y = 1.0;

	result += s * texture(dispTex, pos.xz/waterScale).rgb;

	return result;
}

mat3 calcTBN(vec3 noDispP, vec3 p)
{
	// in x dir
	vec3 px = noDispP + vec3(0.01, 0, 0);
	// in z dir
	vec3 pz = noDispP + vec3(0, 0, 0.01);
	//p = displace(p);
	px = displace(px);
	pz = displace(pz);
	mat3 result;
	result[0] = normalize(pz-p);
	result[1] = normalize(px-p);
	result[2] = cross(result[0], result[1]);
	return result;
}

void main() 
{
	float patchSize = size.x/numPatches;

	vec3 pos;
	// 1.001 to hide seeing through edges
	pos.xz = tcPatchPos + patchSize * gl_TessCoord.xy * 1.001;
	pos.y = 0;

	teNonDisplaceWorld = pos;

	pos = displace(pos);
	//mat3 TBN = calcTBN(teNonDisplaceWorld, pos);
	//mat3 invTBN = transpose(TBN);

	//teTBN = TBN;

	//teUp = invTBN * vec3(0,1,0);
	//teCameraPos = invTBN * cameraPos;
	//teSunDir = invTBN * sunDir;
	//tePosition = invTBN * pos;
	teWPosition = pos;

	teAzimuth = atan(teSunDir.x, teSunDir.z);
	teZenith = acos(teSunDir.y);

	dist = length(cameraPos.xz-pos.xz);

	gl_Position = viewProj * vec4(pos, 1.0);
}