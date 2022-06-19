#version 330 core

in vec2 texCoord_v;

layout(location=0) out vec4 color_out;

uniform sampler2D spriteTex;

uniform vec4 shade;

void main()
{
	color_out = texture(spriteTex, texCoord_v) * shade;
}
