#version 420 core

layout(location=0) in vec2 texCoord_in;
layout(location=0) out vec4 color_out;

layout(binding=0) uniform sampler2D glyph;

uniform vec4 color;

void main()
{
	color_out = color;
	color_out.a = texture(glyph, texCoord_in).r;
}
