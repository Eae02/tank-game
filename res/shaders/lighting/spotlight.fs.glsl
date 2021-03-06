#version 420 core

#include lighting.glh

layout(std140, binding=1) uniform LightUB
{
	vec3 color;
	float intensity;
	float attenLin;
	float attenExp;
	float flickerIntensity;
	float flickerOffset;
	float cutoff;
} light;

uniform vec2 direction;
uniform vec3 position;

layout(location=0) in vec2 worldPos_in;
layout(location=1) in vec2 texCoord_in;

layout(location=0) out vec3 lighting_out;

void main()
{
	GBufferData data = getGBufferData(texCoord_in);
	
	vec3 lightIn = vec3(worldPos_in.x, 0, worldPos_in.y) - position;
	float dist = length(lightIn);
	lightIn /= dist;
	
	float spotFactor = dot(lightIn, normalize(vec3(direction.x, -1, direction.y)));
	
	if (spotFactor > light.cutoff)
	{
		vec3 color = phong(light.intensity, lightIn, eyePosition, worldPos_in, data) * light.color;
		float attenuation = 1.0 + (light.attenLin * dist) + (light.attenExp * dist * dist);
		
		lighting_out = (color / attenuation) * (1.0 - ((1.0 - spotFactor) / (1.0 - light.cutoff)));
		
		lighting_out *= getShadowFactor(worldPos_in);
		lighting_out *= getFlickerFactor(light.flickerOffset, light.flickerIntensity);
	}
	else
	{
		lighting_out = vec3(0.0);
	}
}
