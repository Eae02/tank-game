#version 420 core

#include rendersettings.glh

layout(location=0) out vec4 color_out;

layout(location=0) in vec2 worldPos_in;
layout(location=1) in vec3 texCoord_in;
layout(location=2) in float opacity_in;

layout(binding=0) uniform sampler2D lightingSampler;
layout(binding=1) uniform sampler2DArray diffuseSampler;

layout(std140, binding=2) uniform EmitterSettingsUB
{
	float aspectRatio;
	bool additiveBlend;
};

void main()
{
	color_out = texture(diffuseSampler, texCoord_in);
	
	if (additiveBlend)
	{
		color_out.a = 0.0;
		color_out.rgb *= opacity_in;
	}
	else
	{
		color_out.a *= opacity_in;
		color_out.rgb *= color_out.a;
	}
	
	vec2 screenSamplePos = gl_FragCoord.xy / vec2(resolution);
	color_out.rgb *= texture(lightingSampler, screenSamplePos).rgb;
}
