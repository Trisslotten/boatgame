#version 440 core

layout(points) in;
layout(points, max_vertices = 1) out;

in int vertID[];

flat out int gID;

uniform isampler3D voxels;
uniform mat4 viewProj;
uniform ivec2 viewPortSize;
uniform float fov;


void main()
{
	ivec3 size = textureSize(voxels, 0);
	// https://stackoverflow.com/questions/10903149/how-do-i-compute-the-linear-index-of-a-3d-coordinate-and-vice-versa
	int id = vertID[0];
	ivec3 i;
	i.x = id % size.x;
	id /= size.x;
	i.y = id % size.y;
	i.z = id / size.y;

	int v = texelFetch(voxels, i, 0).x;

	if(v > 0)
	{
		vec3 pos = (vec3(i)*2.0-1.0) / vec3(size) + vec3(0,2,0);
		gl_Position = viewProj * vec4(pos, 1.0);
		vec4 p1 = viewProj * vec4(pos+vec3(0,-.5/size.y, 0), 1.0);
		vec4 p2 = viewProj * vec4(pos+vec3(0, .5/size.y, 0), 1.0);
		gl_PointSize = 1080.0*length(p1.xyz/p1.w - p2.xyz/p2.w);
		gID = vertID[0];
		EmitVertex();
		EndPrimitive();
	}
}