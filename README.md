# Tank Game
A tank shooter written in modern C++ and OpenGL 4.4.

![Ingame Screenshot](https://raw.githubusercontent.com/Eae02/tank-game/master/screenshot.jpg)

## Precompiled binaries
 * [Windows](https://www.dropbox.com/s/aevhdaysm028ur9/tankgame_win32.zip?dl=1). You may also need to install the visual C++ 2015 redistributable (x86 version), which you can get [here](https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe).
 * [Ubuntu (16.10+)](https://www.dropbox.com/s/a4xz1v7vavq5rb7/tankgame_ubuntu.tar.gz?dl=1). You also have to install the runtime dependencies: `sudo apt-get install libglfw3 libopenal1 libfreetype6 libogg0 libvorbis0a libvorbisfile3 zlib1g liblua5.3`.
 * [Arch Linux](https://www.dropbox.com/s/2yyyrogw3lxdj2l/tankgame_arch.tar.gz?dl=1). You also have to install the runtime dependencies: `sudo pacman -S glfw-x11 openal freetype2 libogg libvorbis zlib lua`

The precompiled binaries may work on other linux distributions too, but i haven't tested this.

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
