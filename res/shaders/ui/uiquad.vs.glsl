#version 330 core

uniform vec2 corners[4];

void main()
{
	gl_Position = vec4(corners[gl_VertexID].xy, 0.0, 1.0);
}
