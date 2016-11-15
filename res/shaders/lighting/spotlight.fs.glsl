#version 420 core

#include lighting.glh

layout(std140, binding=1) uniform LightUB
{
	vec3 color;
	float intensity;
	Attenuation attenuation;
	float cutoff;
} light;

uniform vec2 direction;
uniform vec3 position;

layout(location=0) in vec2 worldPos_in;

layout(location=0) out vec3 lighting_out;

void main()
{
	vec2 texCoord = getTexCoord();
	GBufferData data = getGBufferData(texCoord);
	
	vec3 lightIn = vec3(worldPos_in.x, 0, worldPos_in.y) - position;
	float dist = length(lightIn);
	lightIn /= dist;
	
	float spotFactor = dot(lightIn, normalize(vec3(direction.x, -1, direction.y)));
	
	if (spotFactor > light.cutoff)
	{
		vec3 color = phong(light.intensity, lightIn, eyePosition, worldPos_in, data) * light.color;
		float attenuation = 1.0 + (light.attenuation.linear * dist) + (light.attenuation.exponent * dist * dist);
		
		lighting_out = (color / attenuation) * (1.0 - ((1.0 - spotFactor) / (1.0 - light.cutoff)));
		
		lighting_out *= getShadowFactor(worldPos_in);
	}
	else
	{
		lighting_out = vec3(0.0);
	}
}
