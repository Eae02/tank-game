#version 420 core

#include material.glh

layout(std140, binding=1) uniform MaterialUB
{
	vec4 shade;
	float specularIntensity;
	float specularExponent;
};

layout(binding=0) uniform sampler2D diffuseSampler;
layout(binding=1) uniform sampler2D normalMapSampler;

layout(location=0) in vec2 texCoord_in;
layout(location=1) in vec2 worldPos_in;
layout(location=2) in flat mat3 transform_in;

void main()
{
	vec4 color = texture(diffuseSampler, texCoord_in) * shade;
	vec2 normal = toWorldNormal(texture(normalMapSampler, texCoord_in).rgb);
	
	float normalLen = length(normal);
	normal = (transform_in * vec3(normal, 0.0)).xy;
	normal = normalize(normal) * normalLen;
	
	writeMaterialOutputs(color, normal, specularIntensity, specularExponent);
}
