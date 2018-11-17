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
uniform float waveTileSize;
uniform sampler2D waveMap;

uniform sampler2D normalMap;

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


void main()
{

	
	//vec3 normal = normalize(cross(dFdx(teposition), dFdy(teposition)));
	//vec3 normal = calcNormal(teNonDisplace);
	//vec3 normal = normalize(tenormal);

	vec3 texNormal = normalize(texture(normalMap, 0.1*teNonDisplaceWorld.xz + vec2(0,0.05)*time).rgb*2.0 - 1.0);
	vec3 texNormal2 = normalize(texture(normalMap, 0.2*teNonDisplaceWorld.xz + vec2(0.01,0.04)*time).rgb*2.0 - 1.0);
	texNormal = texNormal/2 + texNormal2/2;

	vec3 look = normalize(teCameraPos - tePosition);

	// flatten normal
	float flatness = 0.5 + 3*clamp(1-look.z, 0, 1);
	vec3 normal = normalize(texNormal + vec3(0,0,flatness));
	//normal = vec3(0,0,1);


	//float diffuse = clamp(dot(sun, normal), 0, 1);
	vec3 h = normalize(look + teSunDir);
	float specular = pow(clamp(dot(normal, h), 0.0, 1.0), 2000.0);



	vec3 lighting = vec3(0);

	// from Tessendorf 2001
	vec3 upwelling = vec3(0,0.2,0.4);

	// fake subsurface scattering
	vec3 lookW = normalize(cameraPos - teWPosition);
	upwelling *= 1.0 + 0.7*smoothstep(-1.0, 1.0, teWPosition.y) * pow(max(1.0-lookW.y, 0.0), 2.0);

	vec3 air = vec3(0.1);
	float nSnell = 1.34;
	float Kdiffuse = 0.91;
	float reflectivity;
	vec3 nI = look;
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
	
	vec3 r = reflect(-look, normal);
	// TODO figure out how to do skyColor in tangent space or something
	vec3 worldR = teTBN*r;
	float a = atan(worldR.x, worldR.z);
	float z = acos(worldR.y);

	float g = gamma(z,a);
	z = min(z, 3.1415926/2.0);

	float Yp = Yz * perez(z, g, skyCoeffsY) / perez(0, zenith, skyCoeffsY);
	float xp = xz * perez(z, g, skyCoeffsx) / perez(0, zenith, skyCoeffsx);
	float yp = yz * perez(z, g, skyCoeffsy) / perez(0, zenith, skyCoeffsy);
	
	vec3 skyColor = rgb(Yp, xp, yp);

	/*
	g = gamma(0,0);
	z = 0;
	Yp = Yz * perez(z, g, skyCoeffsY) / perez(0, zenith, skyCoeffsY);
	xp = xz * perez(z, g, skyCoeffsx) / perez(0, zenith, skyCoeffsx);
	yp = yz * perez(z, g, skyCoeffsy) / perez(0, zenith, skyCoeffsy);
	upwelling *= 0.6*length(rgb(Yp, xp, yp));
	*/
	upwelling *= 0.6;


	lighting += reflectivity * skyColor + (1-reflectivity) * upwelling;;
	lighting += specular*sunColor;


	outColor = vec4(lighting,1);
}