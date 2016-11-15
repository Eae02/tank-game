#version 420 core

#include ../rendersettings.glh

layout(location=0) in vec2 position_in;

void main()
{
	gl_Position = vec4((viewMatrix * vec3(position_in, 1.0)).xy, 0, 1);
}
