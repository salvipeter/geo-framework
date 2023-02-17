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

## Changes

Added features:

- More extensible interface

- Handling multiple objects

- Better toolchain (CMake integration, Qt 6, OpenMesh 8)

Removed features:

- Mesh fairing

- Face-based curvature approximation

- Saving Bézier surfaces
