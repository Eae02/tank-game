#version 420 core

#include bloom.glh
#include ../post.glh

layout(location=0) in vec2 texCoord_in;
layout(location=0) out vec3 color_out;

layout(binding=0) uniform sampler2D inputSampler;
layout(binding=1) uniform sampler2D bloomInputSampler;

vec3 sampleInput(vec2 tc)
{
	return texture(bloomInputSampler, tc).rgb;
}

void main()
{
	color_out = getBloomColor(texCoord_in, 1);
	color_out += texture(inputSampler, texCoord_in).rgb;
}
