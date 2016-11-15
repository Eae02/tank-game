#version 420 core

#include shadowrendersettings.glh

layout (lines) in;
layout (triangle_strip) out;
layout (max_vertices=4) out;

layout(location=0) in vec2 position_in[];
layout(location=1) in vec2 normal_in[];

layout(location=0) out float distanceToCaster_out;

float lengthSquared(vec2 v)
{
	return dot(v, v);
}

void main()
{
	vec2 linePos = (position_in[0] + position_in[1]) / 2.0;
	vec2 lineNormal = normalize(normal_in[0] + normal_in[1]);
	
	if (dot(linePos - lightPosition, lineNormal) >= 0)
		return;
	
	float minDistToLightSq = min(lengthSquared(position_in[0] - lightPosition), lengthSquared(position_in[1] - lightPosition));
	if (minDistToLightSq > projectionCircleRadius * projectionCircleRadius)
		return;
	
	for (int i = 0; i < 2; i++)
	{
		gl_Position = vec4((shadowViewMatrix * vec3(position_in[i], 1.0)).xy, 0.0, 1.0);
		distanceToCaster_out = 0;
		EmitVertex();
	}
	
	vec2 projLineDir = vec2(lineNormal.y, -lineNormal.x);
	vec2 projLinePos = lightPosition - lineNormal * projectionCircleRadius;
	vec2 lightToLinePos = projLinePos - lightPosition;
	
	for (int i = 0; i < 2; i++)
	{
		vec2 toVertex = position_in[i] - lightPosition;
		toVertex.y = max(abs(toVertex.y), 1E-6) * (toVertex.y < -1E-6 ? -1 : 1);
		
		float f = toVertex.x / toVertex.y;
		float a = (f * lightToLinePos.y - lightToLinePos.x) / (projLineDir.x - projLineDir.y * f);
		
		//vec2 position = position_in[i] + normalize(toVertex);
		vec2 position = projLinePos + a * projLineDir;
		
		distanceToCaster_out = distance(position, position_in[i]);
		gl_Position = vec4((shadowViewMatrix * vec3(position, 1.0)).xy, 0.0, 1.0);
		EmitVertex();
	}
	
	EndPrimitive();
}
