#include "rendersettings.glh"

layout(location=0) in vec2 position_in;

out vec2 worldPos_v;
out vec3 texCoord_v;
out float opacity_v;

uniform sampler2DArray diffuseSampler;

const int BATCH_SIZE = 256;

layout(std140) uniform ParticlesUB
{
	mat4 transformation[BATCH_SIZE];
	vec4 opacity[BATCH_SIZE / 4];
	ivec4 textureLayers[BATCH_SIZE / 4];
};

uniform float aspectRatio;

void main()
{
	worldPos_v = (transformation[gl_InstanceID] * vec4(position_in.x, position_in.y * aspectRatio, 1.0, 0.0)).xy;
	texCoord_v = vec3((position_in + vec2(1.0)) / 2.0, textureLayers[gl_InstanceID / 4][gl_InstanceID % 4]);
	opacity_v = opacity[gl_InstanceID / 4][gl_InstanceID % 4];
	
	gl_Position = vec4((viewMatrix * vec3(worldPos_v, 1.0)).xy, 0.0, 1.0);
}
