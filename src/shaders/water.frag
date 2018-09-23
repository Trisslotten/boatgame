#version 440 core


out vec4 outColor;

in vec3 teposition;
in vec3 tenormal;

uniform vec3 cameraPos;
uniform float time;

float noise(vec3 p);

float height(vec2 pos)
{
	const vec2 waveDir = normalize(vec2(1,1));
	float sum = 0.0;
	float a = 1.0;
	for(int i = 0; i < 5; i++)
	{
		float n = a * noise(0.3*vec3(pos/a + 2.0*waveDir * time, time));
		sum += n;
		a *= 0.4;
	}
	return sum;
}

vec3 calcNormal(vec3 p)
{
	// in x dir
	vec3 px = p + vec3(0.1, 0, 0);
	// in z dir
	vec3 pz = p + vec3(0, 0, 0.1);
	p.y = height(p.xz);
	px.y = height(px.xz);
	pz.y = height(pz.xz);
	return normalize(cross(pz-p,px-p));
}

void main()
{
	//vec3 normal = normalize(cross(dFdx(teposition), dFdy(teposition)));
	vec3 normal = calcNormal(teposition);

	
	vec3 sun = normalize(vec3(1,1,0));
	float diffuse = clamp(dot(sun, normal), 0, 1);

	vec3 look = normalize(cameraPos - teposition);
	vec3 h = normalize(look + sun);
	float specular = pow(clamp(dot(normal, h), 0.0, 1.0), 1000.0);

	vec3 color = 0.45*vec3(30, 90, 190)/256.0;

	vec3 lighting = vec3(0);
	lighting += color * diffuse * 0.8;
	lighting += specular;
	lighting += color * 0.2;

	outColor = vec4(lighting,1);
}




float mod289(float x){return x - floor(x / 289.0) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x / 289.0) * 289.0;}
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
    vec4 o1 = fract(k3 / 41.0);
    vec4 o2 = fract(k4 / 41.0);
    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);
    return o4.y * d.y + o4.x * (1.0 - d.y);
}