uniform float blastIntensity;
uniform float blastRadius;

in vec2 toOrigin_v;

layout(location=0) out vec2 distortion_out;

const float PI = 3.141592653589793;

void main()
{
	float d = 0.5 * blastIntensity * cos(clamp(blastIntensity * (length(toOrigin_v) - blastRadius), -PI, PI) + PI / 2.0);
	distortion_out = d * normalize(toOrigin_v);
}
