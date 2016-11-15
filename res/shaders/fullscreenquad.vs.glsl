#version 420 core

layout(location=0) in vec2 position_in;

layout(location=0) out vec2 texCoord_out;

void main()
{
	texCoord_out = (position_in + vec2(1.0)) / 2.0;
	gl_Position = vec4(position_in, 0.0, 1.0);
}
