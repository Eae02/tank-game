#version 420 core

#include rendersettings.glh

layout(location=0) in vec2 position_in;

layout(location=0) out vec3 texCoord_out;
layout(location=1) out flat vec2 specular_out;

layout(binding=0) uniform usampler2D tileIDSampler;
layout(binding=1) uniform sampler2D tileRotationSampler;

layout(std140, binding=1) uniform RenderAreaUB
{
	ivec2 offset;
	ivec2 size;
} renderArea;

layout(std140, binding=2) uniform MaterialSettingsUB
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
	ivec2 offset = renderArea.offset + ivec2(gl_InstanceID % renderArea.size.x, gl_InstanceID / renderArea.size.x);
	vec2 worldPos = position_in / 2.0 + vec2(0.5) + vec2(offset);
	
	uint layer = texelFetch(tileIDSampler, offset, 0).r;
	
	vec4 parameters = materialParameters[layer];
	
	//x: cos(rotation), y: sin(rotation)
	vec2 rotation = texelFetch(tileRotationSampler, offset, 0).rg;
	
	texCoord_out = vec3(
		(rotation.x * worldPos.x - rotation.y * worldPos.y) * parameters.x,
		(rotation.y * worldPos.x + rotation.x * worldPos.y) * parameters.y,
		layer
	);
	
	specular_out = parameters.zw;
	
	gl_Position = vec4((viewMatrix * vec3(worldPos, 1.0)).xy, Z, 1.0);
}
