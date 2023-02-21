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
- [CMake](https://cmake.org/) >= 3.2
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
   
   - `QGLVIEWER_LIBRARY` : should be set to the full path to the `libQGLViewerd2.dll` file
     (which was generated in the `QGLViewer` subdirectory of the libQGLViewer *source*)

   Re-configure and build.

To generate a release build, compile both libQGLViewer and OpenMesh in Release mode,
and in the configuration of this framework, change the library filenames to
`libOpenMeshCore.lib` and `libQGLViewer2.dll`, respectively.

## Changes

Added features:

- More extensible interface

- Handling multiple objects

- Better toolchain (CMake integration, Qt 6, OpenMesh 8)

Removed features:

- Mesh fairing

- Face-based curvature approximation

- Saving Bézier surfaces
