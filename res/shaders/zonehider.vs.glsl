#include "rendersettings.glh"

uniform mat3 transform;

layout(location=0) in vec2 position_in;

void main()
{
	gl_Position = vec4((viewMatrix * transform * vec3(position_in, 1.0)).xy, 0.0, 1.0);
}
