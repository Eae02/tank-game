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
} light;

uniform vec3 position;

layout(location=0) in vec2 worldPos_in;
layout(location=1) in vec2 texCoord_in;

layout(location=0) out vec3 lighting_out;

void main()
{
	GBufferData data = getGBufferData(texCoord_in);
	
	vec3 lightIn = vec3(worldPos_in.x, 0, worldPos_in.y) - position;
	float dist = length(lightIn);
	
	lighting_out = phong(light.intensity, lightIn / dist, eyePosition, worldPos_in, data) * light.color;
	
	lighting_out /= 1.0 + (light.attenLin * dist) + (light.attenExp * dist * dist);
	
	lighting_out *= getShadowFactor(worldPos_in);
	lighting_out *= getFlickerFactor(light.flickerOffset, light.flickerIntensity);
}
