#version 440 core

out vec4 outColor;

in vec3 g_pos;


float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

void main()
{	
	vec3 pos = g_pos;
	vec3 normal = normalize(cross(dFdx(g_pos), dFdy(g_pos)));
	vec3 matColor;
	matColor.r = noise(pos);
	matColor.g = noise(pos+10);
	matColor.b = noise(pos+20);
	matColor = vec3(1);

	vec3 sunDir = normalize(vec3(-0.5,2,-1));

	vec3 color = vec3(0);
	color += matColor * max(dot(normal, sunDir), 0);
	color += matColor * 0.2;

	outColor = vec4(color,1);
}