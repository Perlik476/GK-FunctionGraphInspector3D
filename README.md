FunctionGraphInspector3D
========================

This is a fork of OpenGL_CMake_Skeleton. It's readme is available below. The project is written in C++ and uses OpenGL for rendering. The project is built using CMake and uses the GLFW, GLEW, and GLM libraries.

This project is a 3D function graph inspector. It allows the user to input a function from R^2 to R and visualize it's graph in 3D with the ability to rotate, zoom in and out, and move the camera across the graph. Additionally the user can choose a point on the graph which will be the starting point for optimization algorithms. The user can choose between the following algorithms: Gradient Descent and Newton's Method. The app then visualizes the optimization process showing the path the algorithm takes to find the minimum of the function.

Controls
------------------------

- **arrows** - Move the camera
- **left mouse button** - Rotate the camera with the mouse movement
- **right mouse button** - Zoom in and out with the mouse y-axis movement
- **1/2/3** - Set the starting point for the algorithm to the currently selected point on the graph (1 for unselecting the point, 2 for setting the starting point for Newton's Method, 3 for setting the starting point for Gradient Descent)
- **spacebar** - Take a step in the optimization process


OpenGL CMake Skeleton [![Build Status](https://travis-ci.org/ArthurSonzogni/OpenGL_CMake_Skeleton.svg?branch=master)](https://travis-ci.org/ArthurSonzogni/OpenGL_CMake_Skeleton)
=======================

A ready to use C++11 CMake OpenGL skeleton using **GLFW**, **Glew** and **glm**. (use git submodules)

It compiles on:
 * **WebAssembly**  (on branch webassembly. See instruction below)
 * Linux
 * Windows
 * Mac

It can compile for the Web with WebAssembly thanks to emscripten, as well as on
Linux, Windows and Mac.

Shader class and example Application are included.

![output result](output.gif)

I am open to any comments and contributions.

Clone (With submodules):
========================

```
git clone --recursive git@github.com:ArthurSonzogni/OpenGL_CMake_Skeleton.git
```

Alternatively, if you don't used the --recursive option, you can type:
```bash
git submodule init
git submodule update
```

usage (WebAssembly) : 
---------------------
Switch to the webassembly branch
```
git checkout webassembly
```

Install emscripten, then
```bash
mkdir build_emscripten
cd build_emscripten
emcmake cmake ..
make
python -m SimpleHTTPServer 8000
```

Now, visit [http://localhost:8000](http://localhost:8000)

usage (Linux) : 
---------------
Install some standard library, fetch the project, build and run:
```bash
sudo apt-get install cmake libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev
git clone --recursive git@github.com:ArthurSonzogni/OpenGL_CMake_Skeleton.git
cd OpenGL_CMake_Skeleton
mkdir build
cd build
cmake ..
make -j
./opengl-cmake-skeleton
```

usage (Windows) :
-----------------
For instance :
* cmake-gui .
* Configure (Choose for example Visual Studio generator)
* Generate
Launch the generated project in your favorite IDE and run it.
