#include "rendersettings.glh"

layout(location=0) in vec2 position_in;

out vec2 qPosition_v;
out vec2 worldPos_v;

uniform mat3 transform;

void main()
{
	qPosition_v = position_in;
	
	worldPos_v = (transform * vec3(position_in, 1.0)).xy;
	
	gl_Position = vec4((viewMatrix * vec3(worldPos_v, 1.0)).xy, 0.0, 1.0);
}
