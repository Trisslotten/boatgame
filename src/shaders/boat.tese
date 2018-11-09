#version 440 core

layout(triangles, equal_spacing, cw) in;

in vec3 tcpos[];

out vec3 tepos;

uniform mat4 viewProj;


// xz
const vec2 controlTop[4] = 
{
	vec2(0,3), vec2(1,2), vec2(0.6,-1), vec2(0.6,-3)
};
// yz
const vec2 controlSide[4] = 
{
	vec2(0.5,3), vec2(-0.5,2), vec2(-0.5,2), vec2(-0.3,-3)
};

vec2 bezier(float x, vec2 ps[4])
{
	float u = 1.f-x;
	vec2 result = vec2(0);
	result += u*u*u*ps[0];
	result += 3.*u*u*x*ps[1];
	result += 3.*u*x*x*ps[2];
	result += x*x*x*ps[3];
	return result;
}

void main()
{
	
	vec3 pos = vec3(0);
	
	pos += gl_TessCoord[0] * tcpos[0];
	pos += gl_TessCoord[1] * tcpos[1];
	pos += gl_TessCoord[2] * tcpos[2];

	float t = -pos.z/2.0+0.5;
	vec2 top = bezier(t, controlTop);
	top.x = sign(pos.x)*top.x;

	vec2 side = bezier(t, controlSide);

	vec3 p1 = vec3(top.x, pos.y, top.y);
	vec3 p2 = vec3(pos.x, side.x, side.y);
	pos = mix(p2, p1, 1.0-gl_TessCoord[2]);

	pos.y += 5;

	tepos = pos;

	gl_Position = viewProj * vec4(pos, 1.0);	
}