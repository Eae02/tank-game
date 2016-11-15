#version 420 core

#include rendersettings.glh

layout(location=0) in vec2 position_in;

layout(location=0) out vec2 worldPos_out;
layout(location=1) out vec3 texCoord_out;
layout(location=2) out float opacity_out;

layout(binding=1) uniform sampler2DArray diffuseSampler;

const int BATCH_SIZE = 256;

layout(std140, binding=1) uniform ParticlesUB
{
	mat4 transformation[BATCH_SIZE];
	vec4 opacity[BATCH_SIZE / 4];
	ivec4 textureLayers[BATCH_SIZE / 4];
};

layout(std140, binding=2) uniform EmitterSettingsUB
{
	float aspectRatio;
	bool additiveBlend;
};

void main()
{
	worldPos_out = (transformation[gl_InstanceID] * vec4(position_in.x, position_in.y * aspectRatio, 1.0, 0.0)).xy;
	texCoord_out = vec3((position_in + vec2(1.0)) / 2.0, textureLayers[gl_InstanceID / 4][gl_InstanceID % 4]);
	opacity_out = opacity[gl_InstanceID / 4][gl_InstanceID % 4];
	
	gl_Position = vec4((viewMatrix * vec3(worldPos_out, 1.0)).xy, 0.0, 1.0);
}
