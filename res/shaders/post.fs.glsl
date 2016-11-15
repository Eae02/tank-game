#version 420 core

#include post.glh

layout(location=0) in vec2 texCoord_in;

layout(binding=0) uniform sampler2D inputSampler;
layout(binding=1) uniform sampler2D hexagonSampler;
layout(binding=3) uniform sampler1D noiseSampler;

out vec3 color_out;

layout(std140, binding=1) uniform PostSettingsUB
{
	float exposure;
	float gamma;
	float contrast;
	float framebufferAR;
	float damageFlashRadius;
	float damageFlashIntensity;
	float horizontalDistortionAmount;
	float distortionSampleMul;
};

const int NUM_HEX_TEXTURES_Y = 5;
const vec3 DAMAGE_FLASH_COLOR = vec3(232 / 255.0, 0, 58 / 255.0);

void main()
{
	float distortion = texture(noiseSampler, texCoord_in.y * distortionSampleMul).r * horizontalDistortionAmount;
	
	color_out = texture(inputSampler, getDistortedTexCoord(texCoord_in + vec2(distortion, 0.0))).rgb;
	
	color_out = vec3(1.0) - exp(color_out * -vec3(exposure));
	color_out = pow(color_out, vec3(1.0 / gamma));
	
	float lightness = dot(color_out, vec3(0.2126, 0.7152, 0.0722));
	color_out = vec3(lightness) + (color_out - vec3(lightness)) * contrast;
	
	vec2 hexSampleCoord = texCoord_in * vec2(NUM_HEX_TEXTURES_Y * framebufferAR, NUM_HEX_TEXTURES_Y);
	
	float distToCenter = distance(texCoord_in, vec2(0.5)) / length(vec2(0.5));
	
	float hexIntensity = texture(hexagonSampler, hexSampleCoord).r * damageFlashIntensity;
	hexIntensity *= max((distToCenter - damageFlashRadius) / (1.0 - damageFlashRadius), 0.0);
	
	color_out = mix(color_out, DAMAGE_FLASH_COLOR, min(hexIntensity, 1.0));
}
