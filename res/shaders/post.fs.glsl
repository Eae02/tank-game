#version 330 core

in vec2 texCoord_v;

out vec3 color_out;

uniform sampler2D inputSampler;
uniform sampler2D hexagonSampler;
uniform sampler2D distortionSampler;
uniform sampler1D noiseSampler;

vec2 getDistortedTexCoord(vec2 texCoord)
{
	return texCoord + texture(distortionSampler, texCoord).xy / vec2(textureSize(distortionSampler, 0).xy);
}

const float damageFlashRadius = 0.75;

uniform float exposure;
uniform float gamma;
uniform float contrast;
uniform float framebufferAR;
uniform float damageFlashIntensity;
uniform float horizontalDistortionAmount;
uniform float distortionSampleMul;

const int NUM_HEX_TEXTURES_Y = 5;
const vec3 DAMAGE_FLASH_COLOR = vec3(232 / 255.0, 0, 58 / 255.0);

void main()
{
	float distortion = texture(noiseSampler, texCoord_v.y * distortionSampleMul).r * horizontalDistortionAmount;
	
	color_out = texture(inputSampler, getDistortedTexCoord(texCoord_v + vec2(distortion, 0.0))).rgb;
	
	color_out = vec3(1.0) - exp(color_out * -vec3(exposure));
	color_out = pow(color_out, vec3(1.0 / gamma));
	
	float lightness = dot(color_out, vec3(0.2126, 0.7152, 0.0722));
	color_out = vec3(lightness) + (color_out - vec3(lightness)) * contrast;
	
	vec2 hexSampleCoord = texCoord_v * vec2(NUM_HEX_TEXTURES_Y * framebufferAR, NUM_HEX_TEXTURES_Y);
	
	float distToCenter = distance(texCoord_v, vec2(0.5)) / length(vec2(0.5));
	
	float hexIntensity = texture(hexagonSampler, hexSampleCoord).r * damageFlashIntensity;
	hexIntensity *= max((distToCenter - damageFlashRadius) / (1.0 - damageFlashRadius), 0.0);
	
	color_out = mix(color_out, DAMAGE_FLASH_COLOR, min(hexIntensity, 1.0));
}
