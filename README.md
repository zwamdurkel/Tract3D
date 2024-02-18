# Tract-based 3D animation tool

## About

TODO

## External tools

This project uses the following external resources:

| Tool            | Description                                                   | Version | Source                                   | License                                                                  |
|-----------------|---------------------------------------------------------------|---------|------------------------------------------|--------------------------------------------------------------------------|
| GLFW            | An open source, multi-platform library for OpenGL.            | 3.3.9   | [link](https://www.glfw.org/)            | [zlib/libpng](https://www.glfw.org/license.html)                         |
| GLAD            | OpenGL Loader-Generator based on the official specifications. | 0.1.36  | [link](https://glad.dav1d.de/)           | [MIT](https://github.com/Dav1dde/glad?tab=License-1-ov-file#readme)      |
| Dear ImGui      | A bloat-free graphical user interface library.                | 1.90.3  | [link](https://github.com/ocornut/imgui) | [MIT](https://github.com/ocornut/imgui?tab=MIT-1-ov-file#readme)         |
| LearnOpenGL[^1] | Resource for learning OpenGL with examples.                   | N/A     | [link](https://learnopengl.com/)         | [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/legalcode) |

[^1]: Some code has been changed compared to the provided code.

All resources have been included in the repository in `external/` for easy development and building.

## Building

To build the program, use CMake to build the `main` target, or let your IDE figure it out (it probably will).

```bash
mkdir build
cd build
cmake ..
make
```

## Running

The program can be launched in two ways:

1. From a GUI context. E.g: you double-click the file on your desktop. There will be no console output.
2. From a commandline context. There will be console output. When calling from a commandline, please use the following
   to combat asynchronous behaviour:
    - Powershell: `Start-Process -Wait .\main.exe`
    - CMD: `start /wait main.exe`

If you're using an IDE, your IDE will probably do the right thing if you just run the `main` target.