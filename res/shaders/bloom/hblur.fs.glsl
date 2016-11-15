#version 420 core

#include bloom.glh
#include ../post.glh

layout(location=0) in vec2 texCoord_in;
layout(location=0) out vec3 color_out;

layout(binding=0) uniform sampler2D inputSampler;

vec3 sampleInput(vec2 tc)
{
	vec3 val = texture(inputSampler, tc).rgb;
	
	if (dot(val, vec3(0.2126, 0.7152, 0.0722)) < 1)
		return vec3(0.0);
	return val;
}

void main()
{
	color_out = getBloomColor(texCoord_in, 0);
}
