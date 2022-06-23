layout(location=0) in vec2 position_in;

out vec2 texCoord_v;

void main()
{
	texCoord_v = (position_in + vec2(1.0)) / 2.0;
	gl_Position = vec4(position_in, 0.0, 1.0);
}
