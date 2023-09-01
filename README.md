# Geometry Framework

A reorganized (and hopefully better) version of my
[sample framework](https://github.com/salvipeter/sample-framework).

## Usage

Pressing `h` displays a help window that contains all the necessary information.
If you want to use this as a base for your own project, examine the source code
and read the documentation of the various libraries (see "Dependencies" below).

## Installation

Dependencies:

- C++20
- [CMake](https://cmake.org/) >= 3.20
- [libQGLViewer](http://www.libqglviewer.com/) >= 2.9
- [OpenMesh](http://www.openmesh.org/) >= 8.1
- [Qt](http://qt-project.org/) >= 6.4

Optional dependencies:

- [CGAL](https://www.cgal.org/) >= 5.5
- [Eigen](https://eigen.tuxfamily.org/) >= 3.4

### Linux

Standard *CMake* compilation should work.
Note that the *libQGLViewer* library needs to be compiled with Qt6.

### Windows

In this guide we will create a debug build using the MinGW compiler and Qt's Creator IDE, 
but it should work with other configurations, as well.

1. Download [Qt](https://www.qt.io/):

   Select "Download Qt for open source use", and download the online installer tool.
   You will need to register an email address (free).
   For installation type, select "Qt 6.4 for desktop development" (with MinGW toolchain).
   This will install the *MinGW* compiler, the *CMake* build tool,
   and the *Qt Creator* IDE.
   (If you already have a C++ compiler and want to use that, consider downloading
   a smaller package by selecting "Custom".)

1. Download the source of [libQGLViewer](https://libqglviewer.com/).

   Extract it to a suitable folder. Start Qt Creator, and open the project file
   *inside* the `QGLViewer` folder (so its path should be something like
   `libQGLViewer-2.9.1/QGLViewer/QGLViewer.pro`). Build for Debug.
   Close the project.

1. Download the source of [OpenMesh](https://www.openmesh.org/).

   Extract it to a suitable folder. Open the `CMakeLists.txt` file in the
   root directory with Qt Creator. Some errors may appear.
   
   Press the "Project" button on the left sidebar, and go to "Initial configuration".
   Press the "Add..." button, select "Boolean", and enter "BUILD_APPS".
   Leave its value as "OFF", and press the "Re-configure with Initial Parameters" button.
   Check the "OPENMESH_BUILD_SHARED" checkbox, press "Run CMake", and then build.

   (If you get a "file too big" error, go back to the initial configuration,
   press "Add...", select "String", enter "CMAKE_CXX_FLAGS",
   and set the value "-Wa,-mbig-obj", and try to re-configure and build.)

   Close the project.

1. Clone [this project](https://github.com/salvipeter/geo-framework) with git.

   Open the `CMakeLists.txt` file. Some errors may appear.
   
   Press the "Project" button on the left sidebar, and go to "Initial configuration".
   Set the following variables:
   
   - `OPENMESH_INCLUDE_DIR` : should be set to the `src` folder of OpenMesh
   
   - `OPENMESH_LIBRARIES` : should be set to the full path to the `libOpenMeshCored.dll` file
     (which was generated in the `src/OpenMesh/Core` subdirectory of the OpenMesh *build*)
   
   - `QGLVIEWER_INCLUDE_DIR` : should be set to the libQGLViewer source directory
     (something like `libQGLViewer-2.9.1`)
   
   - `QGLVIEWER_LIBRARY` : should be set to the full path to the `QGLViewerd2.dll` file
     (which was generated in the `QGLViewer` subdirectory of the libQGLViewer *source*)

   Re-configure and build.

To generate a release build, compile both libQGLViewer and OpenMesh in Release mode,
and in the configuration of this framework, change the library filenames to
`libOpenMeshCore.dll` and `QGLViewer2.dll`, respectively.

### MacOS

1. Download *OpenMesh*, *Qt6* and *libQGLViewer*.

1. Compile *OpenMesh* with *CMake*, with the `BUILD_APPS` variable set to `OFF`.

1. Compile *libQGLViewer* with *Qt Creator* (similarly as in Windows).

1. Finally, compile this project with *CMake*. Note the following settings:

   - `OPENMESH_LIBRARY_DEBUG`: *<OpenMesh build path>*`/Build/lib/libOpenMeshCore.dylib`

   - `Qt6_DIR`: *<Qt6 install dir>*`/<version>/macos/lib/cmake/Qt6`

   - `QGLVIEWER_LIBRARY_DEBUG`: *<libQGLViewer source path>*`/QGLViewer/QGLViewer.framework`

Since the libQGLViewer framework is local, you should put a symlink beside the executable
for it to work, i.e.

```bash
[/path/to/geo-framework/build] ln -s <libQGLViewer source path>/QGLViewer/QGLViewer.framework .
```

## Notes on adding new object types

A common use-case is to add a new object type that can be (i) read
from file, and (ii) triangulated to form a mesh. By default it will
compute normal vectors and curvatures approximated from the underlying
mesh, but these can be overridden.

Bézier surfaces are supplied as an example, new object types are to be
created in a similar fashion:

1. Create a new class that inherits from Object.

1. Override the triangulating function `updateBaseMesh()`: this is the
main function, where a mesh representation of the surface should be
created. The variable for this is called `mesh`. Also, at the end of
the function, you should call `Object::updateBaseMesh(false, false)`.
Supply `true` arguments when you have special handlers for the normal
or mean curvature, respectively.

1. Optionally override the `normal()` and/or `meanCurvature()`
   functions.

1. Override the main drawing function `draw()`; the surface itself is
drawn based on the triangulation; here you should only draw extra
things, such as the control points etc.

1. Override the file input function `reload()`; it loads the file
given in the `filename` variable. In the end of the function, you
should call `updateBaseMesh()` to generate the surface.

1. In `Window::open()`, add your own file format to the list of
   extensions.

1. In `Viewer::open()`, add a conditional of the type

   ```c++
   else if (filename.ends_with(".your_extension"))
     surface = std::make_shared<YourObjectType>(filename);
   ```
   so that your class will be invoked when a file of the added type is opened.
   (Naturally the `YourObjectType.hh` header should also be included.)

1. Override the modification functions:

   - `drawWithNames()`: Here you should draw only those things that
     you can click on (e.g. control points). Each object is associated
     with a name (an integer).

   - `postSelection()`: returns the position of the selected object
     (`selected` is the name of the object the user clicked on)

   - `movement()`: sets the position of the selected object

## Changes compared to [sample framework](https://github.com/salvipeter/sample-framework)

Added features:

- More extensible interface

- Handling multiple objects

- Better toolchain (CMake integration, Qt 6, OpenMesh 8)

Removed features:

- Mesh fairing

- Face-based curvature approximation

- Saving Bézier surfaces
