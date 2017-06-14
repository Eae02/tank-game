#version 420 core

#include rendersettings.glh

layout(location=0) in vec2 position_in;

layout(location=0) out vec2 qPosition_out;
layout(location=1) out vec2 worldPos_out;

uniform mat3 transform;

void main()
{
	qPosition_out = position_in;
	
	worldPos_out = (transform * vec3(position_in, 1.0)).xy;
	
	gl_Position = vec4((viewMatrix * vec3(worldPos_out, 1.0)).xy, 0.0, 1.0);
}
