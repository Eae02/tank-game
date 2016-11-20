#version 420 core

#include rendersettings.glh

layout(location=0) in vec2 position_in;
layout(location=1) in float angle_in;

layout(location=0) out vec2 worldPos_out;
layout(location=1) out vec2 screenPos_out;
layout(location=2) out float angle_out;

layout(std140, binding=1) uniform ShieldSettingsUB
{
	mat2 transformLin;
	vec2 centerWorld;
	vec2 centerView;
	float radius;
	float intensity;
	
	float rippleCenter;
	float rippleDistance;
	float rippleIntensity;
};

void main()
{
	angle_out = angle_in;
	worldPos_out = transformLin * position_in + centerWorld;
	
	screenPos_out = (viewMatrix * vec3(worldPos_out, 1.0)).xy;
	gl_Position = vec4(screenPos_out, 0.1, 1.0);
}
