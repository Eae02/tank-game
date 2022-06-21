#version 330 core

#include "shadowrendersettings.glh"

layout(location=0) in vec2 position_in;
layout(location=1) in vec3 pairOffsetAndProject_in;

float vertexProjectionDistance = 0;

vec2 project(vec2 pos)
{
	vec2 dir = pos - lightPosition;
	float dst = vertexProjectionDistance / max(length(dir), 1E-5);
	return pos + dir * dst;
}

void main()
{
	vertexProjectionDistance = pairOffsetAndProject_in.z * projectionDistance;
	
	vec2 proj1 = project(position_in);
	vec2 proj2 = project(position_in + pairOffsetAndProject_in.xy);
	vec2 projectedPos = project((proj1 + proj2) / 2.0);
	
	gl_Position = vec4((shadowViewMatrix * vec3(projectedPos, 1.0)).xy, mix(-0.5, 1.0, vertexProjectionDistance), 1.0);
}
