#version 420 core

layout(location=0) in vec2 texCoord_in;
layout(location=0) out vec3 color_out;

layout(binding=0) uniform sampler2D inputSampler;

layout(std140, binding=0) uniform BlurVectorUB
{
	vec2 blurVectors[2];
};

const int SAMPLE_COUNT = 4;
const float KERNEL[] = float[] (
	0.028532, 0.067234, 0.124009, 0.179044, 0.20236, 0.179044, 0.124009, 0.067234, 0.028532
);

#ifdef INITIAL_PASS
const int PASS_INDEX = 0;
#else
const int PASS_INDEX = 1;
#endif

vec3 sampleInput(vec2 tc)
{
	vec3 val = texture(inputSampler, tc).rgb;
	
#ifdef INITIAL_PASS
	if (max(max(val.r, val.g), val.b) < 1 && dot(val, vec3(0.299, 0.587, 0.114)) < 1)
		return vec3(0.0);
#endif
	
	return val;
}

void main()
{
	color_out = sampleInput(texCoord_in).rgb * KERNEL[SAMPLE_COUNT];
	
	for (int i = 1; i <= SAMPLE_COUNT; i++)
	{
		color_out += sampleInput(texCoord_in + blurVectors[PASS_INDEX] * i).rgb * KERNEL[SAMPLE_COUNT - i];
		color_out += sampleInput(texCoord_in - blurVectors[PASS_INDEX] * i).rgb * KERNEL[SAMPLE_COUNT + i];
	}
}
