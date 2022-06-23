in vec2 texCoord_v;

uniform sampler2D diffuseSampler;
uniform sampler2D normalMapSampler;
uniform sampler2D specularMapSampler;

uniform mat3 transform;

#include "material.glh"

const float SPECULAR_vTENSITY = 5.0;
const float SPECULAR_EXPONENT = 30.0;

void main()
{
	vec4 color = texture(diffuseSampler, texCoord_v);
	vec2 normal = toWorldNormal(texture(normalMapSampler, texCoord_v).rgb).xy;
	
	float normalLen = length(normal);
	normal = (transform * vec3(normal, 0.0)).xy;
	normal = normalize(normal) * normalLen;
	
	float specIntensity = texture(specularMapSampler, texCoord_v).r * SPECULAR_vTENSITY;
	
	writeMaterialOutputs(color, makeNormal3(normal), specIntensity, SPECULAR_EXPONENT);
}
