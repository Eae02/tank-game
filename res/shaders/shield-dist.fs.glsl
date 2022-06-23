#include "shield-common.glh"

layout(location=0) out vec2 distortion_out;

const float DISTORTION_vTENSITY = 5.0;

void main()
{
	distortion_out = getShieldIntensity() * DISTORTION_vTENSITY * normalize(centerView - screenPos_v);
}
