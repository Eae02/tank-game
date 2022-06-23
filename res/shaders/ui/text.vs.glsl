layout(location=0) in vec2 position_in;

out vec2 texCoord_v;

uniform vec2 offset;
uniform vec2 size;

void main()
{
	texCoord_v = position_in / 2.0 + vec2(0.5);
	gl_Position = vec4(offset + texCoord_v * size, 0.0, 1.0);
	texCoord_v.y = 1.0 - texCoord_v.y;
}
