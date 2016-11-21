#version 420 core

#include rendersettings.glh

layout(location=0) in vec2 position_in;

layout(location=0) out vec2 texCoord_out;

layout(std140, binding=1) uniform ConveyorSettingsUB
{
	mat3 transform;
	vec2 size;
	float textureOffset;
};

void main()
{
	gl_Position = vec4((viewMatrix * transform * vec3(position_in, 1.0)).xy, 0.79 / 2, 1.0);
	
	texCoord_out = position_in * size + vec2(0.0, textureOffset);
}
