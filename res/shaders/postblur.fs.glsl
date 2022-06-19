#version 330 core

in vec2 texCoord_v;

layout(location=0) out vec4 color_out;

uniform sampler2D inputSampler;

const int SAMPLE_COUNT = 10;
const float KERNEL[] = float[] (
	0.000539, 0.001533, 0.003908, 0.008925, 0.018255, 0.033446, 0.054891, 0.080693, 0.106259, 0.125337, 0.132429, 0.125337, 0.106259, 0.080693, 0.054891, 0.033446, 0.018255, 0.008925, 0.003908, 0.001533, 0.000539
);

uniform vec2 sampleOffset;
uniform vec2 blurVector;

void main()
{
	vec2 centerTexCoord = texCoord_v + sampleOffset;
	color_out = texture(inputSampler, centerTexCoord) * KERNEL[SAMPLE_COUNT];
	
	for (int i = 1; i <= SAMPLE_COUNT; i++)
	{
		color_out += texture(inputSampler, centerTexCoord + blurVector * i) * KERNEL[SAMPLE_COUNT - i];
		color_out += texture(inputSampler, centerTexCoord - blurVector * i) * KERNEL[SAMPLE_COUNT + i];
	}
}
