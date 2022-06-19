#version 330 core

#include "material.glh"

in vec3 texCoord_v;
flat in vec2 specular_v;

uniform sampler2DArray diffuseMaps;
uniform sampler2DArray normalSpecMaps;

void main()
{
	vec4 color = texture(diffuseMaps, texCoord_v);
	vec4 tsNormalAndSpecular = texture(normalSpecMaps, texCoord_v);
	vec3 normal = toWorldNormal(tsNormalAndSpecular.rgb).xzy;
	
	writeMaterialOutputs(color, normal, specular_v.x * tsNormalAndSpecular.a, specular_v.y);
}
