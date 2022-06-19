#version 330 core

#include "rendersettings.glh"

layout(location=0) out vec4 color_out;

in vec2 worldPos_v;
in vec3 texCoord_v;
in float opacity_v;

uniform sampler2D lightingSampler;
uniform sampler2DArray diffuseSampler;

uniform int additiveBlend;

void main()
{
	color_out = texture(diffuseSampler, texCoord_v);
	
	if (additiveBlend == 1)
	{
		color_out.a = 0.0;
		color_out.rgb *= opacity_v;
	}
	else
	{
		color_out.a *= opacity_v;
		color_out.rgb *= color_out.a;
	}
	
	vec2 screenSamplePos = gl_FragCoord.xy / vec2(resolution);
	color_out.rgb *= texture(lightingSampler, screenSamplePos).rgb;
}
