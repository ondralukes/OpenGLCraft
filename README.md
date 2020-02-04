# OpenGLCraft
Minecraft clone written in C++
## Install
Build with CMake

`textures` and `shaders` folders (and `zlib.dll` on Windows) have to be next to the executable

If you want to compile with correct version name on Windows put output of `git describe --tags --abbrev=8 --long --dirty --broken` to CMake variable `OGLC_VERSION`
