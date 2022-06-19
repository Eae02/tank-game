#version 330 core

layout(location=0) in vec2 position_in;

out vec2 texCoord_v;

uniform mat3 transform;
uniform float z;

void main()
{
	texCoord_v = (position_in + vec2(1.0)) / 2.0;
	gl_Position = vec4((transform * vec3(position_in, 0.0)).xy, z, 1.0);
}
