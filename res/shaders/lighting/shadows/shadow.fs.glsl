#version 330 core

in float distanceToCaster_v;

layout(location=0) out float shadowFactor_out;

void main()
{
	shadowFactor_out = distanceToCaster_v;
}
