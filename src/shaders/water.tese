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


// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
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

	float result = o4.y * d.y + o4.x * (1.0 - d.y);
    return result * result;
}

vec3 displace(vec3 pos)
{
	vec3 result = pos;

	vec3 disp = texture(dispTex, pos.xz/waterScale).rgb;

	//float t = 0.1;
	result += disp;// * mix(1.0, t+(1-t)*noise(vec3(0.01*pos.xz, 0.1*time)), min(0.003*length(cameraPos - pos), 1.0));

	//result.y += mix(0.0, 0.2*noise(vec3(0.5*pos.xz,1)), min(0.003*length(cameraPos - pos), 1.0));
	//result.y += mix(0.0, 0.2*noise(vec3(1*pos.xz,1)), min(0.003*length(cameraPos - pos), 1.0));
	//result.y += mix(0.0, 0.2*noise(2*pos.xz), min(0.005*length(cameraPos - pos), 1.0));

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
	pos.xz = tcPatchPos + patchSize * gl_TessCoord.xy * 1.01;
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