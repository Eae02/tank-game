#version 330 core

#include "lighting.glh"

uniform vec3 position;

in vec2 worldPos_v;
in vec2 texCoord_v;

layout(location=0) out vec3 lighting_out;

void main()
{
	GBufferData data = getGBufferData(texCoord_v);
	
	vec3 lightIn = vec3(worldPos_v.x, 0, worldPos_v.y) - position;
	float dist = length(lightIn);
	
	lighting_out = phong(lightIn / dist, eyePosition, worldPos_v, data) * light.color;
	
	lighting_out /= 1.0 + (light.attenLin * dist) + (light.attenExp * dist * dist);
	
	lighting_out *= getShadowFactor(worldPos_v);
	lighting_out *= getFlickerFactor(light.flickerOffset, light.flickerIntensity);
}
