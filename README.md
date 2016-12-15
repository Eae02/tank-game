# Tank Game
A tank shooter written in modern C++ and OpenGL 4.4. The game is still quite far from complete.

## Windows binaries
Precompiled windows binaries are available [here](https://www.dropbox.com/s/aevhdaysm028ur9/tankgame_win32.zip?dl=1). You may also need to install the visual C++ 2015 redistributable, which you can get [here](https://www.microsoft.com/en-us/download/details.aspx?id=48145).

## Compiling from source on Linux
First, make sure you have all the runtime dependencies installed. These are:
* opengl
* openal
* glfw
* glew
* freetype
* ogg
* vorbis
* vorbisfile
* zlib

You will also need CMake and GCC version 5.3 or later to build the game.

1. `mkdir PROJECT_ROOT/.build`
2. `cd PROJECT_ROOT/.build`
3. `cmake -DCMAKE_BUILD_TYPE=Release ..`
4. `cmake --build .`
5. `cp -R ../res ../bin/Release`

The binary will be put in PROJECT_ROOT/bin/Release.

## Compiling from source on Windows
To set up the build system, you will need to install [vcpkg](https://github.com/Microsoft/vcpkg) and the project's runtime dependencies. You will also need to have cmake installed and available on the command line.
After you have installed vcpkg, cd into the install directory and run `.\vcpkg.exe install openal-soft glfw3 glew freetype libogg libvorbis zlib` to install the runtime dependencies.

1. `mkdir PROJECT_ROOT\.build`
2. `cd PROJECT_ROOT\.build`
3. `cmake .. "-DCMAKE_TOOLCHAIN_FILE=VCPKG_PATH\scripts\buildsystems\vcpkg.cmake"`
4. `cmake --build . --config Release`
5. `robocopy ..\res ..\bin\Release\res /E /CREATE`

The binary will be put in PROJECT_ROOT\bin\Release.

## Asset acknowledgements
Some of textures and sprites are self made while others are made by artists on the web, mainly [nobiax](http://nobiax.deviantart.com/) and [jylhis](http://jylhis.deviantart.com/). The ambient music is taken from [InterSpace](http://99sounds.org/interspace).
