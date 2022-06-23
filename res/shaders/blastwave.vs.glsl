#include "rendersettings.glh"

layout(location=0) in vec2 position_in;

uniform vec2 blastOrigin;

out vec2 toOrigin_v;

void main()
{
	toOrigin_v = blastOrigin - (inverseViewMatrix * vec3(position_in, 1.0)).xy;
	gl_Position = vec4(position_in, 0.9, 1.0);
}
