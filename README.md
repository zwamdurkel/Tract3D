# Tract-based 3D animation tool

![img](https://imgur.com/zV7YIX9.png)

## About

The aim of our software is to allow users to view `.tck` files, which represent white matter tracts
derived from diffusion MRI, and engage the user with the help of various effects.
The user will be able to experience a dynamic animation of particles flowing along these white matter
tracts, position the camera along the path of these streamlines and apply a selection of expanding views and
visual enhancements that aid in analyzing and understanding the structures of these white matter tracts.

## External tools

This project uses the following external resources:

| Tool            | Description                                                    | Version | Source                                                    | License                                                                       |
|-----------------|----------------------------------------------------------------|---------|-----------------------------------------------------------|-------------------------------------------------------------------------------|
| GLFW            | An open source, multi-platform library for OpenGL.             | 3.3.9   | [link](https://www.glfw.org/)                             | [zlib](https://www.glfw.org/license.html)                                     |
| GLM             | A header only C++ mathematics library for graphics software.   | 1.0.0   | [link](https://github.com/g-truc/glm)                     | [MIT](https://github.com/g-truc/glm?tab=License-1-ov-file)                    |
| GLAD            | OpenGL Loader-Generator based on the official specifications.  | 0.1.36  | [link](https://glad.dav1d.de/)                            | [MIT](https://github.com/Dav1dde/glad?tab=License-1-ov-file#readme)           |
| Dear ImGui      | A bloat-free graphical user interface library.                 | 1.90.3  | [link](https://github.com/ocornut/imgui)                  | [MIT](https://github.com/ocornut/imgui?tab=MIT-1-ov-file#readme)              |
| NFD Extended    | A library that invokes native file open and file save dialogs. | 1.1.1   | [link](https://github.com/btzy/nativefiledialog-extended) | [zlib](https://github.com/btzy/nativefiledialog-extended/blob/master/LICENSE) |
| LearnOpenGL[^1] | Resource for learning OpenGL with examples.                    | N/A     | [link](https://learnopengl.com/)                          | [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/legalcode)      |

[^1]: Some code has been changed compared to the provided code.

All resources have been included in the repository in `external/` for consistancy during development and building.

## Building

To build the program, use CMake to build the `main` target, or let your IDE figure it out (it probably will).

```bash
mkdir build
cd build
cmake ..
make
```

## Installer

To make an installer for the application (Windows, NSIS), first build the project, then go to the build directory and
run the `cpack` command.

```
cd build
cpack
```

## Running

The program can be launched in two ways:

1. From a GUI context. E.g: you double-click the file on your desktop. There will be no console output.
2. From a commandline context. There will be console output. When calling from a commandline, please use the following
   to combat asynchronous behaviour:
    - Powershell: `Start-Process -Wait .\main.exe`
    - CMD: `start /wait main.exe`

If you're using an IDE, your IDE will probably do the right thing if you just run the `main` target.
