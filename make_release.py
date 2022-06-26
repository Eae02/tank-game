#!/usr/bin/python

#This script packages the executable, res directory and libraries into tankgame_linux.tar.gz.

import tarfile
import os

from elftools.elf.elffile import ELFFile
from elftools.elf.dynamic import DynamicSection

#List of libraries to be included in the archive.
libs = ["glfw", "openal", "ogg", "vorbis", "vorbisfile", "lua", "stdc++"]

binary = "bin/Release-Linux/tankgame"

with tarfile.open("tankgame_linux.tar.gz", "w:gz") as tar:
	tar.add(binary, arcname="tankgame")
	tar.add("res")
	
	elfStream = open(binary, "rb")
	elfFile = ELFFile(elfStream)
	
	#Scans through the dynamic sections of the ELF searching for needed libraries from the libs list.
	for section in elfFile.iter_sections():
		if not isinstance(section, DynamicSection):
			continue
		
		for tag in section.iter_tags():
			if tag.entry.d_tag == "DT_NEEDED" and any(tag.needed.startswith("lib" + lib) for lib in libs):
				tar.add(os.path.realpath("/usr/lib/" + tag.needed), tag.needed)
