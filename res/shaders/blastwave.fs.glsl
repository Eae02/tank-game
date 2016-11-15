#version 420 core

layout(std140, binding=1) uniform BastSettingsUB
{
	vec2 bastOrigin;
	float blastIntensity;
	float blastRadius;
};

layout(location=0) in vec2 worldPos_in;

layout(location=0) out vec2 distortion_out;

const float PI = 3.141592653589793;

void main()
{
	vec2 toOrigin = bastOrigin - worldPos_in;
	
	float d = 0.5 * blastIntensity * cos(clamp(blastIntensity * (length(toOrigin) - blastRadius), -PI, PI) + PI / 2.0);
	distortion_out = d * normalize(toOrigin);
}
