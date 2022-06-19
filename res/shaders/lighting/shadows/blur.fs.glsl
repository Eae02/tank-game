#version 420

#include "shadowrendersettings.glh"

in vec2 texCoord_v;
in vec2 worldPos_v;

uniform sampler2D shadowMap;

layout(location=0) out float shadow_out;

const int SAMPLE_COUNT = 4;
const float KERNEL[] = float[] (0.20236, 0.179044, 0.124009, 0.067234, 0.02853);

void main()
{
	vec2 toLight = lightPosition - worldPos_v;
	
	float distToLight = length(toLight);
	toLight /= distToLight;
	
	vec2 sampleVec = vec2(toLight.y, -toLight.x) * distToLight * 0.1;
	vec2 sampleVecVS = (shadowViewMatrix * vec3(sampleVec, 0.0)).xy;
	
	vec2 texCoordNdc = texCoord_v * 2.0 - vec2(1.0);
	
	shadow_out = texture(shadowMap, texCoord_v).r * KERNEL[0];
	
	for (int i = 1; i <= SAMPLE_COUNT; i++)
	{
		float a = i / float(SAMPLE_COUNT);
		
		vec2 samplePosL = texCoordNdc - sampleVecVS * a;
		vec2 samplePosR = texCoordNdc + sampleVecVS * a;
		
		shadow_out += texture(shadowMap, samplePosL / 2.0 + vec2(0.5)).r * KERNEL[i];
		shadow_out += texture(shadowMap, samplePosR / 2.0 + vec2(0.5)).r * KERNEL[i];
	}
}
