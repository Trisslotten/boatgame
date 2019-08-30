#version 440 core

layout(points) in;
layout(triangle_strip, max_vertices = 14) out;

in int vertID[];

out vec3 g_pos;

uniform isampler3D voxels;
uniform mat4 viewProj;

// https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
const float cube_strip[] = {
    -1.f, 1.f, 1.f,     // Front-top-left
    1.f, 1.f, 1.f,      // Front-top-right
    -1.f, -1.f, 1.f,    // Front-bottom-left
    1.f, -1.f, 1.f,     // Front-bottom-right
    1.f, -1.f, -1.f,    // Back-bottom-right
    1.f, 1.f, 1.f,      // Front-top-right
    1.f, 1.f, -1.f,     // Back-top-right
    -1.f, 1.f, 1.f,     // Front-top-left
    -1.f, 1.f, -1.f,    // Back-top-left
    -1.f, -1.f, 1.f,    // Front-bottom-left
    -1.f, -1.f, -1.f,   // Back-bottom-left
    1.f, -1.f, -1.f,    // Back-bottom-right
    -1.f, 1.f, -1.f,    // Back-top-left
    1.f, 1.f, -1.f      // Back-top-right
};

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
		vec3 voxelSize = 1.0 / vec3(size);
		vec3 voxelPos = (vec3(i)*1.0-0.5) / vec3(size) + vec3(0,0,0);
		for(int j = 0; j < 14; j++)
		{
			vec3 vertPos;
			vertPos.x = cube_strip[3*j];
			vertPos.y = cube_strip[3*j+1];
			vertPos.z = cube_strip[3*j+2];
			vec3 pos = voxelPos + voxelSize * vertPos;
			gl_Position = viewProj * vec4(pos, 1.0);
			g_pos = pos;
			EmitVertex();
		}
		EndPrimitive();
	}
}