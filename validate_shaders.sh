#!/bin/bash
set -e
PREFIX_COMMON="#extension GL_GOOGLE_include_directive : enable"
for f in $(find res/shaders -name "*.glsl"); do
	PREFIX_GLES="#version 300 es\nprecision lowp sampler2DArray;precision lowp usampler2D;\n"
	PREFIX_CORE="#version 330 core\n"
	if [[ $f == *.vs.glsl ]]
	then
		STAGE=vert
	else
		STAGE=frag
		PREFIX_GLES="$PREFIX_GLES\nprecision highp float;\n"
	fi
	
	echo checking gles $f
	printf "$PREFIX_GLES\n$PREFIX_COMMON\n#include \"$f\"\n" | glslangValidator --quiet --stdin -l -S $STAGE "-I$(dirname $f)"
	
	echo checking core $f
	printf "$PREFIX_CORE\n$PREFIX_COMMON\n#include \"$f\"\n" | glslangValidator --quiet --stdin -l -S $STAGE "-I$(dirname $f)"
done
