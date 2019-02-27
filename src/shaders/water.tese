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

float noise(vec2 p);
float noise(float x);
float hash11(float x);

struct WaveData
{
	float speed, amplitude, wavelength, sharpness;
	vec2 waveDir;
};

const WaveData waveData[] = {
	WaveData(3.5, 0.02, 2.6,  0.15,  vec2(0.187664, 0.982233)),
	WaveData(4,   0.02, 3,    0.15,  vec2(0.795801, 0.605558)),
	WaveData(4.5, 0.02, 5,    0.15,  vec2(-0.0361764, 0.999345)),
	WaveData(5,   0.02, 7,    0.15,  vec2(0.910717, 0.413031)),
	WaveData(7,   0.04, 13,   0.15,  vec2(0.187457, 0.982273)),
	WaveData(9,   0.04, 16,   0.15,  vec2(-0.13384, 0.991003)),
	WaveData(10,  0.04, 20,   0.15,  vec2(0.627366, 0.778725)),
	WaveData(9,   0.04, 25,   0.15,  vec2(0.80672, 0.590934)),
	WaveData(15,  0.07, 50,   0.15,  vec2(0.32113, 0.947035)),
	WaveData(14,  0.07, 60,   0.15,  vec2(0.850379, 0.526171)),
	WaveData(40,  0.12, 80,   0.10,  vec2(-0.753498, 0.65745)),
	WaveData(43,  0.12, 100,  0.10,  vec2(0.746627, 0.665243)),
	WaveData(60,  0.15, 180,  0.10,  vec2(0.850194, 0.52647)),
	WaveData(63,  0.15, 200,  0.10,  vec2(-0.620674, 0.784069)),
	WaveData(90,  0.25, 850,  0.10,  vec2(-0.594982, 0.803739)),
	WaveData(103, 0.25, 1000, 0.10,  vec2(0.229604, 0.973284)),
	//WaveData(453, 0.5,  3000, 0.03, vec2(0.0982862, 0.995158)),
	//WaveData(353, 0.5,  2000, 0.03, vec2(0.187664, 0.982233)),
	//WaveData(340, 0.6,  1850, 0.03, vec2(0.171437, 0.985195)),
};


vec3 gerstner(vec2 pos, float S, float A, float L, float Q, vec2 wd)
{
	vec3 result = vec3(0);
	const float pi = 3.1415;
	const float c = sqrt(9.82*2*pi);
	float w = c*inversesqrt(L);
	float Qi = Q/(w*A);
	float ph = S*2.0*pi/L;
	float par = w*dot(wd, pos) + ph * time;
	result.xz += Qi*A*wd*cos(par);
	result.y += A*sin(par);

	float par2 = 0.5*w*dot(mat2(0,1,-1,0)*wd, pos);
	par2 += 2.0*sin(par*0.1);
	result *= (sin(par2)+1.0)*0.5;

	return result;
}

vec3 gerstner(vec2 pos, WaveData d)
{
	return gerstner(pos, d.speed, d.amplitude, d.wavelength, d.sharpness, d.waveDir);
}

vec3 displace(vec3 pos)
{
	vec3 result = vec3(0);

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

	result.xz += pos.xz;
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
	mat3 TBN = calcTBN(teNonDisplaceWorld, pos);
	mat3 invTBN = transpose(TBN);

	teTBN = TBN;

	teUp = invTBN * vec3(0,1,0);
	teCameraPos = invTBN * cameraPos;
	teSunDir = invTBN * sunDir;
	tePosition = invTBN * pos;
	teWPosition = pos;

	teAzimuth = atan(teSunDir.x, teSunDir.z);
	teZenith = acos(teSunDir.y);

	dist = length(cameraPos.xz-pos.xz);

	gl_Position = viewProj * vec4(pos, 1.0);
}

// iq's integer hash https://www.shadertoy.com/view/llGSzw
float uhash11(uint n)
{
	n = (n << 13U) ^ n;
	n = n * (n * n * 15731U + 789221U) + 1376312589U;
	return float(n & 0x7fffffffU) / float(0x7fffffff);
}
float hash11(float x) { return uhash11(uint(int(50.*x) + 1073741824)); }
float uhash12(uvec2 x)
{
	uvec2 q = 1103515245U * ((x >> 1U) ^ (uvec2(x.y, x.x)));
	uint  n = 1103515245U * ((q.x) ^ (q.y >> 3U));
	return float(n) * (1.0 / float(0xffffffffU));
}
float hash12(vec2 x) { return uhash12(uvec2((ivec2(50.*x) + 1073741824))); }
float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	float res = 
		mix(
			mix(hash12(ip), hash12(ip+vec2(1.0,0.0)),u.x),
			mix(hash12(ip+vec2(0.0,1.0)), hash12(ip+vec2(1.0,1.0)),u.x),
		u.y);
	return res;
}

float noise(float x)
{
	float ix = floor(x);
	float u = fract(x);
	u = u*u*(3.0-2.0*u);
	return mix(hash11(ix), hash11(ix+1.0), u);
}