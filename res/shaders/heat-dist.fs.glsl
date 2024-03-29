#include "rendersettings.glh"

const int NUM_SAMPLES = 3;
const float SCROLL_SPEED = 0.6;

in vec2 qPosition_v;
in vec2 worldPos_v;

layout(location=0) out vec2 distortion_out;

uniform sampler2D dudvMap;

uniform float intensity;

layout(std140) uniform TextureMatricesUB
{
	vec4 textureMatrices[NUM_SAMPLES];
};

void main()
{
	distortion_out = vec2(0.0);
	
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		vec2 sampleCoord = mat2(textureMatrices[i].xy, textureMatrices[i].zw) * worldPos_v;
		sampleCoord.x += time * SCROLL_SPEED;
		
		distortion_out += texture(dudvMap, sampleCoord).rg - vec2(0.5);
	}
	
	distortion_out *= (1.0 / float(NUM_SAMPLES)) * intensity * max(1.0 - length(qPosition_v), 0.0);
}
