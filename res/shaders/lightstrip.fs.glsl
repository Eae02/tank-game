#include "material.glh"

uniform vec3 color;

void main()
{
	writeMaterialOutputs(vec4(color, 1.0), vec3(0, 1, 0), 0.0, 0.0);
}
