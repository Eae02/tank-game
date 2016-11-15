#version 420 core

layout(location=0) in vec2 position_in;

layout(location=0) out vec2 texCoord_out;

uniform vec2 sampleRectMin;
uniform vec2 sampleRectMax;

uniform vec2 targetRectMin;
uniform vec2 targetRectMax;

void main()
{
	vec2 posIn01 = (position_in + vec2(1.0)) / 2.0;
	
	texCoord_out = mix(sampleRectMin, sampleRectMax, posIn01);
	texCoord_out.y = 1.0 - texCoord_out.y;
	
	gl_Position = vec4(mix(targetRectMin, targetRectMax, posIn01), 0, 1);
}
