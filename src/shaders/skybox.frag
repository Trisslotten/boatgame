#version 440 core

out vec4 outColor;

in vec3 texCoord;

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
uniform vec3 sunDir;
uniform vec3 sunColor;

uniform float time;
uniform vec2 resolution;



float gamma(float z, float a)
{
	return acos(sin(zenith)*sin(z)*cos(a-azimuth)+cos(zenith)*cos(z));
}
float perez(float z, float g, SkyCoeffs coeffs)
{
	return	(1.0 + coeffs.A*exp(coeffs.B/cos(z)))*
			(1.0 + coeffs.C*exp(coeffs.D*g)+coeffs.E*pow(cos(g), 2.0));
}
vec3 rgb(float Y, float x, float y)
{
	float X = x/y*Y;
	float Z = (1.-x-y)/y*Y;
	vec3 result;
	result.r =  3.2406f * X - 1.5372f * Y - 0.4986f * Z;
	result.g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
	result.b =  0.0557f * X - 0.2040f * Y + 1.0570f * Z;
	return result;
}

float uhash12(uvec2 x)
{
	uvec2 q = 1103515245U * ((x >> 1U) ^ (uvec2(x.y, x.x)));
	uint  n = 1103515245U * ((q.x) ^ (q.y >> 3U));
	return float(n) * (1.0 / float(0xffffffffU));
}
float hash12(vec2 x) { return uhash12(uvec2(50.*x)); }
vec3 dither(vec3 color) {
	color.r += mix(-0.5/255.0, 0.5/255.0, hash12(gl_FragCoord.xy));
	color.g += mix(-0.5/255.0, 0.5/255.0, hash12(gl_FragCoord.xy + 1000));
	color.b += mix(-0.5/255.0, 0.5/255.0, hash12(gl_FragCoord.xy + 2000));
	return color;
}

vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

void main() 
{
	vec3 dir = normalize(texCoord);

	float a = atan(dir.x, dir.z);
	float z = acos(dir.y);

	float g = gamma(z,a);
	z = min(z, 3.1415926/2.0);

	float Yp = Yz * perez(z, g, skyCoeffsY) / perez(0, zenith, skyCoeffsY);
	float xp = xz * perez(z, g, skyCoeffsx) / perez(0, zenith, skyCoeffsx);
	float yp = yz * perez(z, g, skyCoeffsy) / perez(0, zenith, skyCoeffsy);



	float dist = length(sunDir - dir);

	outColor = vec4(rgb(Yp, xp, yp), 1);

	//outColor.rgb *= 0.7;

	// sun
	outColor.rgb = mix(outColor.rgb, sunColor, smoothstep(0.2, 0.03, dist));


	outColor.rgb = dither(outColor.rgb);

	//outColor.rgb = fract(outColor.rgb);
}