#version 330 core

#include "../rendersettings.glh"

layout(location=0) in vec2 position_in;

uniform mat3 worldTransform;

out vec2 worldPos_v;
out vec2 texCoord_v;

void main()
{
	worldPos_v = (worldTransform * vec3(position_in, 1.0)).xy;
	
	gl_Position = vec4((viewMatrix * vec3(worldPos_v, 1.0)).xy, 0.0, 1.0);
	texCoord_v = (gl_Position.xy + vec2(1)) / 2;
}
