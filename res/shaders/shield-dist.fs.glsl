#version 420 core

#include shield-common.glh

layout(location=0) out vec2 distortion_out;

const float DISTORTION_INTENSITY = 5;

void main()
{
	distortion_out = getShieldIntensity() * DISTORTION_INTENSITY * normalize(centerView - screenPos_in);
}
