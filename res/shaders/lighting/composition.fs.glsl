#version 420 core

layout(binding=0) uniform sampler2D colorSampler;
layout(binding=1) uniform sampler2D lightAccSampler;

layout(location=0) in vec2 texCoord_in;

layout(location=0) out vec3 color_out;

void main()
{
	color_out = texture(colorSampler, texCoord_in).rgb * texture(lightAccSampler, texCoord_in).rgb;
}
