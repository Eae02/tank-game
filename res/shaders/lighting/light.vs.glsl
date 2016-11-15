#version 420 core

#include ../rendersettings.glh

layout(location=0) in vec2 position_in;

uniform mat3 worldTransform;

layout(location=0) out vec2 worldPos_out;

void main()
{
	worldPos_out = (worldTransform * vec3(position_in, 1.0)).xy;
	
	gl_Position = vec4((viewMatrix * vec3(worldPos_out, 1.0)).xy, 0.0, 1.0);
}
