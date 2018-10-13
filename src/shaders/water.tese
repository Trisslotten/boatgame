#version 440 core

layout(quads, equal_spacing, cw) in;

patch in vec2 tcPatchPos;

// tangent space
out vec3 tePosition;
out vec3 teCameraPos;
out vec3 teSunDir;

// world space
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
	float w = sqrt(9.82*2*3.1415/L);
	float Qi = Q/(w*A);
	float ph = S*2*3.1415/L;
	float par = w*dot(wd, pos) + ph * time;
	//par += 20.0*noise(pos*0.002 + 100*hash11(i));
	result.xz += Qi*A*wd*cos(par);
	result.y += A*sin(par);

	float par2 = 0.3*w*dot(mat2(0,1,-1,0)*wd, pos) + 10*hash11(i+100);
	result *= pow((sin(par2)+1)/2, 3);
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
	float ws = 1.0;
	
	for(int i = 0; i < 3; i++)
	{
		float amp = 1.5;
		vec2 waveDir = rotate(windDir, ws*(hash11(i)-0.5));
		float a = 1/pow(2,i);
		result += gerstner(pos.xz, 1000*a, amp*a, 5000*a, 0.1, waveDir, i);

		
		waveDir = rotate(windDir, ws*(hash11(i+10)-0.5));
		result += gerstner(pos.xz, 900*a, amp*a, 4500*a, 0.1, waveDir, i+10);

		waveDir = rotate(windDir, ws*(hash11(i+20)-0.5));
		result += gerstner(pos.xz, 800*a, amp*a, 4000*a, 0.1, waveDir, i+20);

		waveDir = rotate(windDir, ws*(hash11(i+30)-0.5));
		result += gerstner(pos.xz, 700*a, amp*a, 3500*a, 0.1, waveDir, i+30);
		
	}
	
	for(int i = 0; i < 5; i++)
	{
		vec2 waveDir = rotate(windDir, ws*(hash11(i)-0.5));
		float a = 1/pow(1.3,i);
		result += gerstner(pos.xz, 20*a, 0.05*a, 20*a, 0.2, waveDir, i);

		waveDir = rotate(windDir, ws*(hash11(i+10)-0.5));
		result += gerstner(pos.xz, 18*a, 0.05*a, 18*a,0.2, waveDir, i+10);

		waveDir = rotate(windDir, ws*(hash11(i+20)-0.5));
		result += gerstner(pos.xz, 16*a, 0.05*a, 16*a, 0.2, waveDir, i+20);

		waveDir = rotate(windDir, ws*(hash11(i+30)-0.5));
		result += gerstner(pos.xz, 14*a, 0.05*a, 14*a, 0.2, waveDir, i+30);

	}
	return result;
}

mat3 calcTBN(vec3 p)
{
	// in x dir
	vec3 px = p + vec3(0.02, 0, 0);
	// in z dir
	vec3 pz = p + vec3(0, 0, 0.02);
	p = displace(p);
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
	mat3 TBN = calcTBN(teNonDisplaceWorld);
	mat3 invTBN = transpose(TBN);

	teTBN = TBN;

	teCameraPos = invTBN * cameraPos;
	//teSunDir = invTBN * sunDir;
	tePosition = invTBN * pos;

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
float hash11(float x) { return uhash11(uint(50.*x)); }
float uhash12(uvec2 x)
{
	uvec2 q = 1103515245U * ((x >> 1U) ^ (uvec2(x.y, x.x)));
	uint  n = 1103515245U * ((q.x) ^ (q.y >> 3U));
	return float(n) * (1.0 / float(0xffffffffU));
}
float hash12(vec2 x) { return uhash12(uvec2(50.*x)); }
float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	float res = mix(
		mix(hash12(ip),hash12(ip+vec2(1.0,0.0)),u.x),
		mix(hash12(ip+vec2(0.0,1.0)),hash12(ip+vec2(1.0,1.0)),u.x),u.y);
	return res;
}