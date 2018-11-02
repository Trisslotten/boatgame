#include "water.hpp"

// iq's integer hash https://www.shadertoy.com/view/llGSzw
float uhash11(glm::uint n)
{
	n = (n << 13U) ^ n;
	n = n * (n * n * 15731U + 789221U) + 1376312589U;
	return float(n & 0x7fffffffU) / float(0x7fffffff);
}
float uhash12(glm::uvec2 x)
{
	glm::uvec2 q = 1103515245U * ((x >> 1U) ^ (glm::uvec2(x.y, x.x)));
	glm::uint  n = 1103515245U * ((q.x) ^ (q.y >> 3U));
	return float(n) * (1.0 / float(0xffffffffU));
}
float hash11(float x) { return uhash11(glm::uint(50.f*x)); }
float hash12(glm::vec2 x) { return uhash12(glm::uvec2(50.f*x)); }
float noise(glm::vec2 p)
{
	glm::vec2 ip = glm::floor(p);
	glm::vec2 u = glm::fract(p);
	u = u * u*(3.0f - 2.0f*u);

	float res = glm::mix(
		glm::mix(hash12(ip), hash12(ip + glm::vec2(1.0f, 0.0f)), u.x),
		glm::mix(hash12(ip + glm::vec2(0.0f, 1.0f)), hash12(ip + glm::vec2(1.0f, 1.0f)), u.x), u.y);
	return 2.f * res*res - 1.f;
}
glm::vec2 rotate(glm::vec2 v, float a)
{
	float s = sin(a);
	float c = cos(a);
	glm::mat2 m = glm::mat2(c, -s, s, c);
	return m * v;
}
glm::vec3 gerstner(glm::vec2 pos, float S, float A, float L, float Q, glm::vec2 wd, float i, float time)
{
	glm::vec3 result = glm::vec3(0);
	float w = sqrt(9.82 * 2 * 3.1415 / L);
	float Qi = Q / (w*A);
	float ph = S * 2 * 3.1415 / L;
	float par = w * dot(wd, pos) + ph * time;
	//par += 20.0*noise(pos*0.002 + 100*hash11(i));
	result.x += Qi * A*wd.x*cos(par);
	result.z += Qi * A*wd.y*cos(par);
	result.y += A * sin(par);

	float par2 = 0.3f*w*glm::dot(glm::mat2(0, 1, -1, 0)*wd, pos) + 10.f * hash11(i + 100);
	result *= pow((sin(par2) + 1) / 2, 3);

	return result;
}

glm::vec3 displace(glm::vec3 pos, float time)
{
	glm::vec2 windDir = glm::normalize(glm::vec2(0, 1));
	glm::vec3 result;
	result.x = pos.x;
	result.z = pos.z;
	result.y = 0;
	// wave spread 
	float ws = 1.0;
	for (int i = 0; i < 3; i++)
	{
		float amp = 0.8;
		float a = 1.0 / (i + 1.0);
		glm::vec2 waveDir = rotate(windDir, ws*(hash11(i) - 0.5));
		result += gerstner(glm::vec2(pos.x, pos.z), 1000 * a, amp*a, 5000 * a, 0.1, waveDir, i, time);

		waveDir = rotate(windDir, ws*(hash11(i + 10) - 0.5));
		result += gerstner(glm::vec2(pos.x, pos.z), 900 * a, amp*a, 4500 * a, 0.1, waveDir, i + 10, time);

		waveDir = rotate(windDir, ws*(hash11(i + 20) - 0.5));
		result += gerstner(glm::vec2(pos.x, pos.z), 800 * a, amp*a, 4000 * a, 0.1, waveDir, i + 20, time);

		waveDir = rotate(windDir, ws*(hash11(i + 30) - 0.5));
		result += gerstner(glm::vec2(pos.x, pos.z), 700 * a, amp*a, 3500 * a, 0.1, waveDir, i + 30, time);
	}
	return result;
}


float Water::heightAt(const glm::vec3& pos, float time)
{
	return displace(pos, time).y;
}

glm::vec3 Water::velocityAt(const glm::vec3 & pos, float time)
{
	float timestep = 1.0f/60.0f;
	glm::vec3 now = displace(pos, time);
	glm::vec3 future = displace(pos, time + timestep);

	return (future - now) / timestep;
}
