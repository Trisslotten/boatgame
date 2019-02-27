#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 v_pos[];
out vec3 g_pos;

void main()
{
	vec3 v1 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	vec3 n = abs(normalize(cross(v1, v2)));
	float m = max(n.x, max(n.y, n.z));
	for(int i = 0; i < 3; i++)
	{
		gl_Position.xyzw = gl_in[i].gl_Position.xyzw;
		if(m == n.x)
			gl_Position.xyzw = gl_in[i].gl_Position.zyxw;
		else if(m == n.y)
			gl_Position.xyzw = gl_in[i].gl_Position.xzyw;
		g_pos = v_pos[i];
		EmitVertex();
	}
	EndPrimitive();
}