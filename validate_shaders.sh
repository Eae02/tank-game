#!/bin/bash
set -e
for f in $(find res/shaders -name "*.glsl"); do
	if [[ $f == *.vs.glsl ]]
	then
		STAGE=vert
	else
		STAGE=frag
	fi
	
	echo checking $f
	
	(head -n1 $f;printf "#extension GL_GOOGLE_include_directive : enable\n#line 2\n";tail -n+2 $f) |
		glslangValidator --quiet --stdin -l -S $STAGE "-I$(dirname $f)"
done
