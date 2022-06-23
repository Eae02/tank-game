#include "rendersettings.glh"

layout(location=0) in vec2 position_in;

out vec2 texCoord_v;

uniform mat3 transform;
uniform vec2 size;
uniform float texCoordOffset;

void main()
{
	gl_Position = vec4((viewMatrix * transform * vec3(position_in, 1.0)).xy, 0.79 / 2.0, 1.0);
	
	texCoord_v = position_in * size + vec2(0.0, texCoordOffset);
}
