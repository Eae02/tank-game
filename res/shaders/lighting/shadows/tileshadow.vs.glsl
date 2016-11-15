#version 420 core

layout(location=0) in vec2 position_in;
layout(location=1) in vec2 normal_in;

layout(location=0) out vec2 position_out;
layout(location=1) out vec2 normal_out;

void main()
{
	position_out = position_in;
	normal_out = normal_in;
}
