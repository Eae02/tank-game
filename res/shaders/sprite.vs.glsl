#version 330 core

#include "rendersettings.glh"

layout(location=0) in vec2 position_in;
layout(location=1) in float z_in;
layout(location=2) in vec4 texArea_in;
layout(location=3) in mat3 transform_in;

out vec2 texCoord_v;
out vec2 worldPos_v;
flat out mat3 transform_v;

void main()
{
	texCoord_v = mix(texArea_in.xy, texArea_in.zw, (position_in + vec2(1.0)) / 2.0);
	worldPos_v = (transform_in * vec3(position_in, 1.0)).xy;
	transform_v = transform_in;
	
	gl_Position = vec4((viewMatrix * vec3(worldPos_v, 1.0)).xy, z_in / 2, 1.0);
}
