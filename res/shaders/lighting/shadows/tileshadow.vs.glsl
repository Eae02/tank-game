#version 330 core

#include "shadowrendersettings.glh"

layout(location=0) in vec2 position_in;

out float distanceToCaster_v;

void main()
{
	vec2 position = position_in;
	
	if ((gl_VertexID & 2) != 0)
	{
		vec2 toVertex = position_in - lightPosition;
		toVertex.y = max(abs(toVertex.y), 1E-6) * (toVertex.y < -1E-6 ? -1 : 1);
		position = position_in + normalize(toVertex) * projectionDistance;
	}
	
	distanceToCaster_v = distance(position, position_in);
	gl_Position = vec4((shadowViewMatrix * vec3(position, 1.0)).xy, 0.0, 1.0);
}
