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

//https://gist.github.com/yiwenl/3f804e80d0930e34a0b33359259b556c
vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}
vec3 gerstner(vec2 pos, float S, float A, float L, float Q, vec2 wd, float i)
{
	vec3 result = vec3(0);
	const float pi = 3.1415;
	const float c = sqrt(9.82*2*pi);
	float w = c*inversesqrt(L);
	float Qi = Q/(w*A);
	float ph = S*2.0*pi/L;
	float par = w*dot(wd, pos) + ph * time;
	//par += 20.0*noise(pos*0.002 + 100*hash11(i));
	result.xz += Qi*A*wd*cos(par);
	result.y += A*sin(par);

	float par2 = 0.3*w*dot(mat2(0,1,-1,0)*wd, pos);
	//par2 += 20.0*noise(par*0.02 + 0.3*time);
	par2 += 5.0*sin(par*0.1);

	result *= (sin(par2)+1.1)*0.5;
	//result *= (sin(0.1*w*dot(wd, pos))+1)/2;
	return result;
}

vec3 displace(vec3 pos)
{
	vec2 windDir = normalize(vec2(0,1));
	vec3 result;
	result.xz = pos.xz;
	result.y = 0;

	// wave spread 
	float ws = 2.0;
	const int stride = 50;
	
	int i = 0;
	vec2 waveDir;

	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 5, 0.02, 7, 0.2, waveDir, i);	
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 4.5, 0.02, 5, 0.2, waveDir, i);
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 4, 0.02, 3, 0.2, waveDir, i);	
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 3.5, 0.02, 2.6, 0.2, waveDir, i);
	

	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 7, 0.04, 13, 0.2, waveDir, i);
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 9, 0.04, 16, 0.2, waveDir, i);

	
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 10, 0.04, 20, 0.1, waveDir, i);
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 9, 0.04, 25, 0.1, waveDir, i);
	
	
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 15, 0.07, 50, 0.1, waveDir, i);
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 14, 0.07, 60, 0.1, waveDir, i);
	
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 43, 0.12, 100, 0.1, waveDir, i);
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 40, 0.12, 80, 0.1, waveDir, i);
	
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 63, 0.15, 200, 0.1, waveDir, i);
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 60, 0.15, 180, 0.1, waveDir, i);


	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 103, 0.25, 1000, 0.1, waveDir, i);
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 90, 0.25, 850, 0.1, waveDir, i);
	

	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 353, 0.5, 2000, 0.01, waveDir, i);
	i += stride; waveDir = rotate(windDir, ws*(hash11(i)-0.5));
	result += gerstner(pos.xz, 340, 0.6, 1850, 0.01, waveDir, i);
	

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
	pos.xz = tcPatchPos + patchSize * gl_TessCoord.xy;
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