#version 440 core

out int vertID;

void main()
{
	vertID = gl_VertexID;
}