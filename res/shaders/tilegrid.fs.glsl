#version 420 core

#include material.glh

layout(location=0) in vec3 texCoord_in;
layout(location=1) in flat vec2 specular_in;

layout(binding=2) uniform sampler2DArray diffuseMaps;
layout(binding=3) uniform sampler2DArray normalMaps;
layout(binding=4) uniform sampler2DArray specularMaps;

void main()
{
	vec4 color = texture(diffuseMaps, texCoord_in);
	vec3 normal = toWorldNormal(texture(normalMaps, texCoord_in).rgb).xzy;
	
	writeMaterialOutputs(color, normal, specular_in.x * texture(specularMaps, texCoord_in).r, specular_in.y);
}
