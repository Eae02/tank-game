#version 420 core

layout(location=0) in vec2 position_in;

layout(location=0) out vec2 texCoord_out;

uniform vec2 offset;
uniform vec2 size;

void main()
{
	texCoord_out = position_in / 2.0 + vec2(0.5);
	gl_Position = vec4(offset + texCoord_out * size, 0.0, 1.0);
	texCoord_out.y = 1.0 - texCoord_out.y;
}
