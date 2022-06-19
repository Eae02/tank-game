#version 330 core

uniform sampler2D colorSampler;
uniform sampler2D lightAccSampler;

in vec2 texCoord_v;

layout(location=0) out vec3 color_out;

void main()
{
	color_out = texture(colorSampler, texCoord_v).rgb * texture(lightAccSampler, texCoord_v).rgb;
}
