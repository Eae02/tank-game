#version 420 core

layout(location=0) in vec3 position_in;
layout(location=1) in vec2 texCoord_in;

layout(location=0) out vec2 texCoord_out;

void main()
{
	texCoord_out = texCoord_in;
	gl_Position = vec4(position_in.xy, 0.0, position_in.z);
}
