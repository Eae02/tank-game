#include "shield-common.glh"

layout(location=0) out vec4 color_out;

uniform vec3 color;

void main()
{
	float alpha = getShieldIntensity();
	color_out = vec4(color * alpha, alpha);
}
