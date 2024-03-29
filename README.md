# Tank Game
A tank shooter written in modern C++ and OpenGL.

Browser version available [here](https://eae02.github.io/tankgame/) (requires WebGL 2.0).

![Ingame Screenshot](https://raw.githubusercontent.com/Eae02/tank-game/master/screenshot.jpg)

## Precompiled binaries
 * [Windows](https://www.dropbox.com/s/aevhdaysm028ur9/tankgame_win32.zip?dl=1).
 * [Linux](https://www.dropbox.com/s/u0wxsf4hvspbdxv/tankgame_linux.tar.gz?dl=1). Some of the runtime dependencies are included in the archive. Not included are zlib, glib, xlib, and freetype. Install these with your package manager if you don't already have them.

## Compiling from source
### 1) Clone the repo
```bash
git clone https://github.com/Eae02/tank-game.git
cd tank-game
```
### 2) Install the dependencies
On linux: Install the runtime dependencies (glfw 3.2, openal, freetype, libogg, libvorbis, libvorbisfile, zlib, lua 5.3) along with their headers. You will also need glm, cmake and gcc (version 5.3 or later) to build the game.

On windows: Your best option is to use [vcpkg](https://github.com/Microsoft/vcpkg). Once you have vcpkg, install the dependencies by running `vcpkg.exe install opengl openal-soft glfw3 freetype libogg libvorbis zlib glm lua`. You will also need to have cmake installed and available from the command line.

### 3) Compile
Create the build directory:
```bash
mkdir .build
cd .build
```
On linux:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cp -R ../res ../bin/Release
```
On windows (substitute VCPKG_PATH with the path to where you installed vcpkg):
```bash
cmake .. "-DCMAKE_TOOLCHAIN_FILE=VCPKG_PATH\scripts\buildsystems\vcpkg.cmake"
cmake --build . --config Release
robocopy ..\res ..\bin\Release\res /E /CREATE
```

The binary will be put in bin\Release.

## Asset acknowledgements
Some of textures and sprites are self made while others are made by artists on the web, mainly [nobiax](http://nobiax.deviantart.com/) and [jylhis](http://jylhis.deviantart.com/). The ambient music is taken from [InterSpace](http://99sounds.org/interspace).
