#ifndef LAYER_COUNT
#define LAYER_COUNT 8
#endif

#include "rendersettings.glh"

layout(location=0) in vec2 position_in;

out vec3 texCoord_v;
flat out vec2 specular_v;

uniform usampler2D tileIDSampler;
uniform sampler2D tileRotationSampler;

uniform ivec2 renderAreaOffset;
uniform int renderAreaWidth;

layout(std140) uniform MaterialSettingsUB
{
	/*
		x: Texture Scale X
		y: Texture Scale Y
		z: Specular Intensity
		w: Specular Exponent
	*/
	vec4 materialParameters[LAYER_COUNT];
};

const float Z = 1.0;

void main()
{
	ivec2 offset = renderAreaOffset + ivec2(gl_InstanceID % renderAreaWidth, gl_InstanceID / renderAreaWidth);
	vec2 worldPos = position_in / 2.0 + vec2(0.5) + vec2(offset);
	
	uint layer = texelFetch(tileIDSampler, offset, 0).r;
	
	vec4 parameters = materialParameters[layer];
	
	//x: cos(rotation), y: sin(rotation)
	vec2 rotation = texelFetch(tileRotationSampler, offset, 0).rg;
	
	texCoord_v = vec3(
		(rotation.x * worldPos.x - rotation.y * worldPos.y) * parameters.x,
		(rotation.y * worldPos.x + rotation.x * worldPos.y) * parameters.y,
		layer
	);
	
	specular_v = parameters.zw;
	
	gl_Position = vec4((viewMatrix * vec3(worldPos, 1.0)).xy, Z, 1.0);
}
