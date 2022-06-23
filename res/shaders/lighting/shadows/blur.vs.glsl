layout(location=0) in vec2 position_in;

out vec2 texCoord_v;
out vec2 worldPos_v;

uniform mat3 inverseViewMatrix;

void main()
{
	texCoord_v = (position_in + vec2(1.0)) / 2.0;
	
	worldPos_v = (inverseViewMatrix * vec3(position_in, 1.0)).xy;
	
	gl_Position = vec4(position_in, 0.0, 1.0);
}
