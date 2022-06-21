#version 330 core

#include "rendersettings.glh"
#include "shield-settings.glh"

layout(location=0) in vec3 positionAngle_in;

out vec2 worldPos_v;
out vec2 screenPos_v;
out float angle_v;

void main()
{
	angle_v = positionAngle_in.z;
	worldPos_v = mat2(transformRS.x, transformRS.y, -transformRS.y, transformRS.x) * positionAngle_in.xy + centerWorld;
	
	screenPos_v = (viewMatrix * vec3(worldPos_v, 1.0)).xy;
	gl_Position = vec4(screenPos_v, 0.1, 1.0);
}
