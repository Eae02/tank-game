uniform vec2 vertices[2];

void main()
{
	gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
}
