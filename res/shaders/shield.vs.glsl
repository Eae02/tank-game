#version 420 core

#include rendersettings.glh

layout(location=0) in vec2 position_in;
layout(location=1) in float angle_in;

layout(location=0) out vec2 worldPos_out;
layout(location=1) out float angle_out;

layout(std140, binding=1) uniform ShieldSettingsUB
{
	mat2 transformLin;
	vec2 center;
	float radius;
	float rippleProgress;
	float rippleIntensity;
};

void main()
{
	angle_out = angle_in;
	worldPos_out = transformLin * position_in + center;
	
	gl_Position = vec4((viewMatrix * vec3(worldPos_out, 1.0)).xy, 0.0, 1.0);
}
