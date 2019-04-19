#version 440 core

out vec4 outColor;

// tangent space
in vec3 tePosition;
in vec3 teCameraPos;
in vec3 teSunDir;
in vec3 teUp;
in float teAzimuth;
in float teZenith;

// world space
in vec3 teWPosition;
in vec3 teNonDisplaceWorld;
in float dist;
in mat3 teTBN;

uniform vec3 cameraPos;
uniform vec3 size;
uniform vec3 sunColor;
uniform float time;
uniform vec3 sunDir;

struct SkyCoeffs
{
	float A, B, C, D, E;
};
uniform float turbidity;
uniform float zenith;
uniform float azimuth;
uniform float Yz;
uniform float xz;
uniform float yz;
uniform SkyCoeffs skyCoeffsY;
uniform SkyCoeffs skyCoeffsx;
uniform SkyCoeffs skyCoeffsy;

uniform float waterSizeScale;

uniform sampler2D dispTex;
uniform sampler2D normalMap;

const float PI = 3.1415926;

float gamma(float z, float a)
{
	return acos(sin(zenith)*sin(z)*cos(a-azimuth)+cos(zenith)*cos(z));
	//return acos(sin(teZenith)*sin(z)*cos(a-teAzimuth)+cos(teZenith)*cos(z));
}
float perez(float z, float g, SkyCoeffs coeffs)
{
	return	(1 + coeffs.A*exp(coeffs.B/cos(z)))*
			(1 + coeffs.C*exp(coeffs.D*g)+coeffs.E*pow(cos(g), 2));
}
vec3 rgb(float Y, float x, float y)
{
	float X = x/y*Y;
	float Z = (1-x-y)/y*Y;
	vec3 result;
	result.r =  3.2406f * X - 1.5372f * Y - 0.4986f * Z;
	result.g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
	result.b =  0.0557f * X - 0.2040f * Y + 1.0570f * Z;
	return result;
}




struct WaveData
{
	float speed, amplitude, wavelength, sharpness;
	vec2 waveDir;
};

const WaveData waveData[] = {
	WaveData(1.5, 0.02, 2.6,  0.05,  vec2(0.187664, 0.982233)),
	WaveData(2,   0.02, 3,    0.08,  vec2(0.795801, 0.605558)),
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
	result += waterSizeScale*texture(dispTex, pos.xz/waterSizeScale).rgb;
	return result;
}

vec3 calcNormal()
{
	float t = waterSizeScale/textureSize(dispTex, 0).x;

	// in x dir
	vec3 px = teNonDisplaceWorld + vec3(t, 0, 0);
	// in z dir
	vec3 pz = teNonDisplaceWorld + vec3(0, 0, t);
	vec3 p = displace(teNonDisplaceWorld);
	px = displace(px);
	pz = displace(pz);
	mat3 result;
	vec3 v1 = normalize(pz-p);
	vec3 v2 = normalize(px-p);
	return cross(v1, v2);
}

void main()
{
	//vec3 normal = normalize(cross(dFdx(teposition), dFdy(teposition)));
	//vec3 normal = normalize(tenormal);

	vec3 normal = calcNormal();

	/*
	vec3 texNormal = normalize(texture(normalMap, 0.03*teNonDisplaceWorld.xz + vec2(0,0.01)*time).rgb*2.0 - 1.0);
	vec3 texNormal2 = normalize(texture(normalMap, 0.2*teNonDisplaceWorld.zx + vec2(0.01,0.04)*time).rgb*2.0 - 1.0);
	texNormal = (texNormal + texNormal2)*0.5;

	vec3 look = normalize(teCameraPos - tePosition);

	// flatten normal
	float flatness = 1.5 + 3*clamp(1-look.z, 0, 1);
	//vec3 normal = normalize(texNormal + vec3(0,0,flatness));
	//normal = vec3(0,0,1);
	*/

	//float diffuse = clamp(dot(sun, normal), 0, 1);
	//vec3 h = normalize(look + teSunDir);
	//float specular = pow(clamp(dot(normal, h), 0.0, 1.0), 2000.0);

	
	vec3 lighting = vec3(0);

	// from Tessendorf 2001
	vec3 upwelling = vec3(0,0.2,0.4);

	// fake subsurface scattering
	vec3 lookW = normalize(cameraPos - teWPosition);
	upwelling *= 1.0 + 0.7*smoothstep(-1.0, 1.0, teWPosition.y) * pow(max(1.0-lookW.y, 0.0), 2.0);


	vec3 h = normalize(lookW + sunDir);
	float specular = 50*pow(clamp(dot(normal, h), 0.0, 1.0), 1000.0);


	vec3 air = vec3(0.1);
	float nSnell = 1.34;
	float Kdiffuse = 0.91;
	float reflectivity;
	vec3 nI = lookW;
	vec3 nN = normal;
	float costhetai = abs(dot(nI, nN));
	float thetai = acos(costhetai);
	float sinthetat = sin(thetai)/nSnell;
	float thetat = asin(sinthetat);
	if(thetai == 0.0) 
	{
		reflectivity = (nSnell - 1)/(nSnell + 1);
		reflectivity = reflectivity * reflectivity;
	}
	else
	{
		float fs = sin(thetat - thetai) / sin(thetat + thetai);
		float ts = tan(thetat - thetai) / tan(thetat + thetai);
		reflectivity = 0.5 * (fs*fs + ts*ts);
	}
	//float d = length(cameraPos - teposition) * Kdiffuse;
	//d = exp(-d);
	float d = 1.0;
	
	vec3 r = reflect(-lookW, normal);
	// TODO figure out how to do skyColor in tangent space or something
	vec3 worldR = r;//teTBN*r;
	float a = atan(worldR.x, worldR.z);
	float z = acos(worldR.y);

	float g = gamma(z,a);
	float rz = z;
	z = min(z, PI/2.0);

	float Yp = Yz * perez(z, g, skyCoeffsY) / perez(0, zenith, skyCoeffsY);
	float xp = xz * perez(z, g, skyCoeffsx) / perez(0, zenith, skyCoeffsx);
	float yp = yz * perez(z, g, skyCoeffsy) / perez(0, zenith, skyCoeffsy);
	
	vec3 skyColor = rgb(Yp, xp, yp);
	//skyColor = pow(skyColor, vec3(0.8));
	

	float t = smoothstep(PI/2.0, 0.8*PI/2.0, rz);
	skyColor = upwelling * t + skyColor * (1.0-t);


	/*
	g = gamma(0,0);
	z = 0;
	Yp = Yz * perez(z, g, skyCoeffsY) / perez(0, zenith, skyCoeffsY);
	xp = xz * perez(z, g, skyCoeffsx) / perez(0, zenith, skyCoeffsx);
	yp = yz * perez(z, g, skyCoeffsy) / perez(0, zenith, skyCoeffsy);
	upwelling *= 0.6*length(rgb(Yp, xp, yp));
	*/
	upwelling *= 0.4;


	lighting += reflectivity * skyColor + (1-reflectivity) * upwelling;;
	lighting += specular*sunColor;


	float exposure = -1.0;
	lighting = 1.0 - exp(lighting * exposure);

	outColor = vec4(lighting,1);

	//outColor = vec4(normal,1);

	//outColor = vec4(vec3(5*waterSizeScale*texture(dispTex, teNonDisplaceWorld.xz/waterSizeScale).g), 1);
}