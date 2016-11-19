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

uniform vec2 position;
uniform vec2 direction;
uniform float height;

layout(location=0) in vec2 worldPos_in;

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
	vec2 texCoord = getTexCoord();
	GBufferData data = getGBufferData(texCoord);
	
	vec2 lightIn2D = worldPos_in - getClosestPointOnLightLine(worldPos_in);
	vec3 lightIn = vec3(lightIn2D.x, -height, lightIn2D.y);
	float dist = length(lightIn);
	
	lighting_out = phong(light.intensity, lightIn / dist, eyePosition, worldPos_in, data) * light.color;
	
	lighting_out /= 1.0 + (light.attenLin * dist) + (light.attenExp * dist * dist);
	
	lighting_out *= getFlickerFactor(light.flickerOffset, light.flickerIntensity);
}
