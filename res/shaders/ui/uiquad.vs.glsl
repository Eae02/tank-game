#version 420 core

layout(location=0) in vec2 position_in;

uniform mat3 transform;

void main()
{
	gl_Position = vec4((transform * vec3(position_in, 1.0)).xy, 0.0, 1.0);
}
