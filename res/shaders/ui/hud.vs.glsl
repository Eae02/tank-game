#version 330 core

layout(location=0) in vec3 position_in;
layout(location=1) in vec2 texCoord_in;

out vec2 texCoord_v;

void main()
{
	texCoord_v = texCoord_in;
	gl_Position = vec4(position_in.xy, 0.0, position_in.z);
}
