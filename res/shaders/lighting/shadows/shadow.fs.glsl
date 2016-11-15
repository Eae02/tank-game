#version 420 core

layout(location=0) in float distanceToCaster_in;

layout(location=0) out float shadowFactor_out;

void main()
{
	shadowFactor_out = distanceToCaster_in;
}
