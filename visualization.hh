#pragma once

#if !defined(APIENTRY) && defined(WIN32)
#define APIENTRY __stdcall
#endif

#if !defined(WINGDIAPI)
#define WINGDIAPI
#endif

#if __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "base-mesh.hh"

enum class VisType { PLAIN, MEAN, SLICING, ISOPHOTES };

struct Visualization {
  Visualization();

  // Flags
  VisType type;
  bool show_control_points, show_solid, show_wireframe;

  // Mean curvature
  double mean_min, mean_max, cutoff_ratio;

  // Slicing
  Vector slicing_dir;
  double slicing_scaling;

  // Textures
  static GLuint isophote_texture, environment_texture, slicing_texture;
  GLuint current_isophote_texture;

  // Utilities
  static Vector colorMap(double min, double max, double d);
};
