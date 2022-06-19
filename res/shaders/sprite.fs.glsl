#version 330 core

#include "material.glh"

uniform vec4 shade;
uniform float specularIntensity;
uniform float specularExponent;

uniform sampler2D diffuseSampler;
uniform sampler2D normalMapSampler;

in vec2 texCoord_v;
in vec2 worldPos_v;
flat in mat3 transform_v;

void main()
{
	vec4 color = texture(diffuseSampler, texCoord_v) * shade;
	vec2 normal = toWorldNormal(texture(normalMapSampler, texCoord_v).rgb).xy;
	
	float normalLen = length(normal);
	normal = (transform_v * vec3(normal, 0.0)).xy;
	normal = normalize(normal) * normalLen;
	
	writeMaterialOutputs(color, makeNormal3(normal), specularIntensity, specularExponent);
}
