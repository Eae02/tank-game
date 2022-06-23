#include "lighting.glh"

uniform vec2 direction;
uniform vec3 position;

in vec2 worldPos_v;
in vec2 texCoord_v;

layout(location=0) out vec3 lighting_out;

void main()
{
	GBufferData data = getGBufferData(texCoord_v);
	
	vec3 lightIn = vec3(worldPos_v.x, 0, worldPos_v.y) - position;
	float dist = length(lightIn);
	lightIn /= dist;
	
	float spotFactor = dot(lightIn, normalize(vec3(direction.x, -1, direction.y)));
	float cutoff = light.extra;
	
	if (spotFactor > cutoff)
	{
		vec3 color = phong(lightIn, eyePosition, worldPos_v, data) * light.color;
		float attenuation = 1.0 + (light.attenLin * dist) + (light.attenExp * dist * dist);
		
		lighting_out = (color / attenuation) * (1.0 - ((1.0 - spotFactor) / (1.0 - cutoff)));
		
		lighting_out *= getShadowFactor(worldPos_v);
		lighting_out *= getFlickerFactor(light.flickerOffset, light.flickerIntensity);
	}
	else
	{
		lighting_out = vec3(0.0);
	}
}
