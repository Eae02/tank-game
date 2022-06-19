#version 330 core

#include "lighting.glh"

uniform vec2 position;
uniform vec2 direction;
uniform float height;

in vec2 worldPos_v;
in vec2 texCoord_v;

layout(location=0) out vec3 lighting_out;

float getProjectionMul(vec2 v, vec2 projTarget)
{
	return dot(projTarget, v) / dot(projTarget, projTarget);
}

vec2 getClosestPointOnLightLine(vec2 point)
{
	float p = getProjectionMul(point - position, direction);
	return position + direction * clamp(p, 0, 1);
}

void main()
{
	GBufferData data = getGBufferData(texCoord_v);
	
	vec2 lightIn2D = worldPos_v - getClosestPointOnLightLine(worldPos_v);
	vec3 lightIn = vec3(lightIn2D.x, -height, lightIn2D.y);
	float dist = length(lightIn);
	
	lighting_out = phong(lightIn / dist, eyePosition, worldPos_v, data) * light.color;
	
	lighting_out /= 1.0 + (light.attenLin * dist) + (light.attenExp * dist * dist);
	
	lighting_out *= getFlickerFactor(light.flickerOffset, light.flickerIntensity);
}
