#version 420 core

#include material.glh

layout(std140, binding=1) uniform LightStripUB
{
	vec3 color;
};

void main()
{
	writeMaterialOutputs(vec4(color, 1.0), vec2(0.0), 0.0, 0.0);
}
