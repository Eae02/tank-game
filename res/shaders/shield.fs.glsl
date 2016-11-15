#version 420 core

layout(location=0) in vec2 worldPos_in;
layout(location=1) in float angle_in;

layout(location=0) out vec2 distortion_out;

layout(std140, binding=1) uniform ShieldSettingsUB
{
	mat2 transformLin;
	vec2 center;
	float radius;
	float rippleProgress;
	float rippleIntensity;
};

const float PI = 3.141592653589793;
const float INTENSITY = 20;

void main()
{
	vec2 toCenter = center - worldPos_in;
	
	float d = 0.5 * INTENSITY * cos(clamp(INTENSITY * (length(toCenter) - radius), -PI, PI) + PI / 2.0);
	
	d *= 1.0 - abs(angle_in - rippleProgress) * rippleIntensity;
	
	distortion_out = d * normalize(toCenter) * 0.3;
}
