#include <fstream>

#include "bezier.hh"

Bezier::Bezier(std::string filename) : Object(filename) {
  reload();
}

Bezier::~Bezier() {
}

void Bezier::draw(const Visualization &vis) const {
  if (vis.show_control_points) {
    glDisable(GL_LIGHTING);
    glLineWidth(3.0);
    glColor3d(0.3, 0.3, 1.0);
    size_t m = degree[1] + 1;
    for (size_t k = 0; k < 2; ++k)
      for (size_t i = 0; i <= degree[k]; ++i) {
        glBegin(GL_LINE_STRIP);
        for (size_t j = 0; j <= degree[1-k]; ++j) {
          size_t const index = k ? j * m + i : i * m + j;
          const auto &p = control_points[index];
          glVertex3dv(p.data());
        }
        glEnd();
      }
    glLineWidth(1.0);
    glPointSize(8.0);
    glColor3d(1.0, 0.0, 1.0);
    glBegin(GL_POINTS);
    for (const auto &p : control_points)
      glVertex3dv(p.data());
    glEnd();
    glPointSize(1.0);
    glEnable(GL_LIGHTING);
  }
}

void Bezier::drawWithNames(const Visualization &vis) const {
  if (!vis.show_control_points)
    return;
  for (size_t i = 0; i < control_points.size(); ++i) {
    const auto &p = control_points[i];
    glPushName(i);
    glRasterPos3dv(p.data());
    glPopName();
  }
}

Vector Bezier::postSelection(int selected) {
  return control_points[selected];
}

void Bezier::movement(int selected, const Vector &pos) {
  control_points[selected] = pos;
}

static void bernstein(size_t n, double u, std::vector<double> &coeff) {
  coeff.clear(); coeff.reserve(n + 1);
  coeff.push_back(1.0);
  double u1 = 1.0 - u;
  for (size_t j = 1; j <= n; ++j) {
    double saved = 0.0;
    for (size_t k = 0; k < j; ++k) {
      double tmp = coeff[k];
      coeff[k] = saved + tmp * u1;
      saved = tmp * u;
    }
    coeff.push_back(saved);
  }
}

void Bezier::update() {
  size_t resolution = 50;

  mesh.clear();
  std::vector<BaseMesh::VertexHandle> handles, tri;
  size_t n = degree[0], m = degree[1];

  std::vector<double> coeff_u, coeff_v;
  for (size_t i = 0; i < resolution; ++i) {
    double u = (double)i / (double)(resolution - 1);
    bernstein(n, u, coeff_u);
    for (size_t j = 0; j < resolution; ++j) {
      double v = (double)j / (double)(resolution - 1);
      bernstein(m, v, coeff_v);
      Vector p(0.0, 0.0, 0.0);
      for (size_t k = 0, index = 0; k <= n; ++k)
        for (size_t l = 0; l <= m; ++l, ++index)
          p += control_points[index] * coeff_u[k] * coeff_v[l];
      handles.push_back(mesh.add_vertex(p));
    }
  }
  for (size_t i = 0; i < resolution - 1; ++i)
    for (size_t j = 0; j < resolution - 1; ++j) {
      tri.clear();
      tri.push_back(handles[i * resolution + j]);
      tri.push_back(handles[i * resolution + j + 1]);
      tri.push_back(handles[(i + 1) * resolution + j]);
      mesh.add_face(tri);
      tri.clear();
      tri.push_back(handles[(i + 1) * resolution + j]);
      tri.push_back(handles[i * resolution + j + 1]);
      tri.push_back(handles[(i + 1) * resolution + j + 1]);
      mesh.add_face(tri);
    }
}

bool Bezier::reload() {
  size_t n, m;
  try {
    std::ifstream f(filename.c_str());
    f.exceptions(std::ios::failbit | std::ios::badbit);
    f >> n >> m;
    degree[0] = n++; degree[1] = m++;
    control_points.resize(n * m);
    for (size_t i = 0, index = 0; i < n; ++i)
      for (size_t j = 0; j < m; ++j, ++index)
        f >> control_points[index][0] >> control_points[index][1] >> control_points[index][2];
  } catch(std::ifstream::failure &) {
    return false;
  }
  return true;
}
