in vec2 texCoord_v;

layout(location=0) out vec4 color_out;

uniform sampler2D spriteTex;

uniform vec4 shade;

#ifdef GL_ES
uniform int redToAlpha;
#endif

void main()
{
	color_out = texture(spriteTex, texCoord_v);
#ifdef GL_ES
	if (redToAlpha == 1)
		color_out = vec4(1.0, 1.0, 1.0, color_out.r);
#endif
	color_out *= shade;
}
