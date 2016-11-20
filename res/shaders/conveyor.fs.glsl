#version 420 core

layout(location=0) in vec2 texCoord_in;

layout(binding=0) uniform sampler2D diffuseSampler;
layout(binding=1) uniform sampler2D normalMapSampler;
layout(binding=2) uniform sampler2D specularMapSampler;

layout(std140, binding=1) uniform ConveyorSettingsUB
{
	mat3 transform;
	vec2 size;
	float textureOffset;
};

#include material.glh

const float SPECULAR_INTENSITY = 5;
const float SPECULAR_EXPONENT = 30;

void main()
{
	vec4 color = texture(diffuseSampler, texCoord_in);
	vec2 normal = toWorldNormal(texture(normalMapSampler, texCoord_in).rgb);
	
	float normalLen = length(normal);
	normal = (transform * vec3(normal, 0.0)).xy;
	normal = normalize(normal) * normalLen;
	
	float specIntensity = texture(specularMapSampler, texCoord_in).r * SPECULAR_INTENSITY;
	
	writeMaterialOutputs(color, normal, specIntensity, SPECULAR_EXPONENT);
}
