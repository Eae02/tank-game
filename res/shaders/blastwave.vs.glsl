#version 420 core

#include rendersettings.glh

layout(location=0) in vec2 position_in;

layout(location=0) out vec2 worldPos_out;

void main()
{
	worldPos_out = (inverseViewMatrix * vec3(position_in, 1.0)).xy;
	gl_Position = vec4(position_in, 0.9, 1.0);
}
