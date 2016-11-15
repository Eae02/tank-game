#version 420 core

layout(location=0) in vec2 texCoord_in;

layout(location=0) out vec4 color_out;

layout(binding=0) uniform sampler2D inputSampler;

void main()
{
	color_out = texture(inputSampler, texCoord_in);
}
