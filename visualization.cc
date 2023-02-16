#include "visualization.hh"

Visualization::Visualization() :
  type(VisType::PLAIN),
  show_control_points(true), show_solid(true), show_wireframe(false),
  mean_min(0.0), mean_max(0.0), cutoff_ratio(0.05),
  slicing_dir(0, 0, 1), slicing_scaling(1)
{
}

GLuint Visualization::isophote_texture,
  Visualization::environment_texture,
  Visualization::slicing_texture;

static Vector HSV2RGB(Vector hsv) {
  // As in Wikipedia
  double c = hsv[2] * hsv[1];
  double h = hsv[0] / 60;
  double x = c * (1 - std::abs(std::fmod(h, 2) - 1));
  double m = hsv[2] - c;
  Vector rgb(m, m, m);
  if (h <= 1)
    return rgb + Vector(c, x, 0);
  if (h <= 2)
    return rgb + Vector(x, c, 0);
  if (h <= 3)
    return rgb + Vector(0, c, x);
  if (h <= 4)
    return rgb + Vector(0, x, c);
  if (h <= 5)
    return rgb + Vector(x, 0, c);
  if (h <= 6)
    return rgb + Vector(c, 0, x);
  return rgb;
}

Vector Visualization::colorMap(double min, double max, double d) {
  double red = 0, green = 120, blue = 240; // Hue
  if (d < 0) {
    double alpha = min ? std::min(d / min, 1.0) : 1.0;
    return HSV2RGB({green * (1 - alpha) + blue * alpha, 1, 1});
  }
  double alpha = max ? std::min(d / max, 1.0) : 1.0;
  return HSV2RGB({green * (1 - alpha) + red * alpha, 1, 1});
}
