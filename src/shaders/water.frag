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

uniform float waterScale;

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



vec3 displace(vec3 pos)
{
	vec3 result = pos;
	result += texture(dispTex, pos.xz/waterScale).rgb;
	return result;
}

vec3 calcNormal()
{
	float t = 1.5*waterScale/textureSize(dispTex, 0).x;

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
	return normalize(cross(v1, v2));
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
	//upwelling *= 1.0 + 0.7*smoothstep(-1.0, 1.0, teWPosition.y) * pow(max(1.0-lookW.y, 0.0), 2.0);


	vec3 h = normalize(lookW + sunDir);
	float specular = 100*pow(clamp(dot(normal, h), 0.0, 1.0), 10000.0);


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
	//skyColor = upwelling * t + skyColor * (1.0-t);


	/*
	g = gamma(0,0);
	z = 0;
	Yp = Yz * perez(z, g, skyCoeffsY) / perez(0, zenith, skyCoeffsY);
	xp = xz * perez(z, g, skyCoeffsx) / perez(0, zenith, skyCoeffsx);
	yp = yz * perez(z, g, skyCoeffsy) / perez(0, zenith, skyCoeffsy);
	upwelling *= 0.6*length(rgb(Yp, xp, yp));
	*/
	//upwelling *= 0.4;


	lighting += reflectivity * skyColor + (1-reflectivity) * upwelling;;
	lighting += specular*sunColor;


	float exposure = -1.0;
	lighting = 1.0 - exp(lighting * exposure);

	outColor = vec4(lighting,1);

	//outColor = vec4(normal,1);

	//outColor = vec4(vec3(5*waterSizeScale*texture(dispTex, teNonDisplaceWorld.xz/waterSizeScale).g), 1);
}