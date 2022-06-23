in vec2 texCoord_v;

layout(location=0) out vec4 color_out;

uniform sampler2D glyph;

uniform vec4 color;

void main()
{
	color_out = color;
	color_out.a *= texture(glyph, texCoord_v).r;
}
