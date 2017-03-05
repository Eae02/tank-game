# Tank Game
A tank shooter written in modern C++ and OpenGL 4.4. The game is not done yet, but it is certainly in a playable state.

![Ingame Screenshot](https://raw.githubusercontent.com/Eae02/tank-game/master/screenshot.jpg)

## Precompiled binaries
 * [Windows](https://www.dropbox.com/s/aevhdaysm028ur9/tankgame_win32.zip?dl=1). You may also need to install the visual C++ 2015 redistributable (x86 version), which you can get [here](https://www.microsoft.com/en-us/download/details.aspx?id=48145).
 * [Linux](https://www.dropbox.com/s/2yyyrogw3lxdj2l/tankgame_linux.tar.gz?dl=1)

On linux you will need to install the runtime dependencies through your package manager. These are:
* openal
* glfw (at least version 3.2. If you're on Ubuntu, this is only available in 16.10.)
* freetype
* ogg
* vorbis
* vorbisfile
* zlib
* lua (at least version 5.2)

On ubuntu: `sudo apt-get install libglfw3 libopenal1 libfreetype6 libogg0 libvorbis0a libvorbisfile3 zlib1g liblua5.2`

On arch: `sudo pacman -S glfw-x11 openal freetype2 libogg libvorbis zlib lua`

## Compiling from source
### 1) Clone the repo
```bash
git clone https://github.com/Eae02/tank-game.git
cd tank-game
```
### 2) Install the dependencies
On linux: Install the runtime dependencies listed above along with their headers. In addition to that you will also need glm, cmake and gcc (version 5.3 or later).

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
