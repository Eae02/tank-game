# Tank Game
A tank shooter written in modern C++ and OpenGL. You will need at least OpenGL 4.2 with the GL_ARB_direct_state_access extension to run the game.
The game is still very early in development. The play button is not currently hooked up but you can load the first level by pressing tilde (the button under escape) and typing `level level_1` in the console.

## Windows binaries
Precompiled windows binaries are available [here](https://www.dropbox.com/s/aevhdaysm028ur9/tankgame_win32.zip?dl=1). You may also need to install the visual C++ 2015 and OpenAL redistributables. Which you can get [here](https://www.microsoft.com/en-us/download/details.aspx?id=48145) and [here](https://www.openal.org/downloads/oalinst.zip).

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
4. `make`
5. `cp -R ../res ../bin`

The binary will be put in PROJECT_ROOT/bin.

## Asset acknowledgements
Some of textures and sprites are self made while others are made by artists on the web, mainly [nobiax](http://nobiax.deviantart.com/) and [jylhis](http://jylhis.deviantart.com/). The ambient music is taken from [InterSpace](http://99sounds.org/interspace).
