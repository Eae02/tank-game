#version 330 core

#include "bloom.glh"

in vec2 texCoord_v;

layout(location=0) out vec3 color_out;

uniform sampler2D inputSampler;
uniform sampler2D bloomInputSampler;

vec3 sampleInput(vec2 tc)
{
	return texture(bloomInputSampler, tc).rgb;
}

void main()
{
	color_out = getBloomColor(texCoord_v, 1);
	color_out += texture(inputSampler, texCoord_v).rgb;
}
