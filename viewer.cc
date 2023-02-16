#include <QtGui/QKeyEvent>

#include "mesh.hh"
#include "viewer.hh"

Viewer::Viewer(QWidget *parent) : QGLViewer(parent) {
  setSelectRegionWidth(10);
  setSelectRegionHeight(10);
  axes.shown = false;
}

double Viewer::getCutoffRatio() const {
  return vis.cutoff_ratio;
}

void Viewer::setCutoffRatio(double ratio) {
  vis.cutoff_ratio = ratio;
  updateMeanMinMax();
}

double Viewer::getMeanMin() const {
  return vis.mean_min;
}

void Viewer::setMeanMin(double min) {
  vis.mean_min = min;
}

double Viewer::getMeanMax() const {
  return vis.mean_max;
}

void Viewer::setMeanMax(double max) {
  vis.mean_max = max;
}

const double *Viewer::getSlicingDir() const {
  return vis.slicing_dir.data();
}

void Viewer::setSlicingDir(double x, double y, double z) {
  vis.slicing_dir = Vector(x, y, z).normalized();
}

double Viewer::getSlicingScaling() const {
  return vis.slicing_scaling;
}

void Viewer::setSlicingScaling(double scaling) {
  vis.slicing_scaling = scaling;
}

void Viewer::deleteObjects() {
  objects.clear();
}

bool Viewer::openMesh(const std::string &filename, bool update_view) {
  auto mesh = std::make_shared<Mesh>(filename);
  if (!mesh->valid())
    return false;
  objects.push_back(mesh);
  if (update_view) {
    updateMeanMinMax();
    setupCamera();
  }
  return true;
}

bool Viewer::openBezier(const std::string &filename, bool update_view) {
  // size_t n, m;
  // try {
  //   std::ifstream f(filename.c_str());
  //   f.exceptions(std::ios::failbit | std::ios::badbit);
  //   f >> n >> m;
  //   degree[0] = n++; degree[1] = m++;
  //   control_points.resize(n * m);
  //   for (size_t i = 0, index = 0; i < n; ++i)
  //     for (size_t j = 0; j < m; ++j, ++index)
  //       f >> control_points[index][0] >> control_points[index][1] >> control_points[index][2];
  // } catch(std::ifstream::failure &) {
  //   return false;
  // }
  // model_type = ModelType::BEZIER_SURFACE;
  // last_filename = filename;
  // updateMesh(update_view);
  // if (update_view)
  //   setupCamera();
  return true;
}

void Viewer::init() {
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

  QImage img(":/isophotes.png");
  glGenTextures(1, &vis.isophote_texture);
  glBindTexture(GL_TEXTURE_2D, vis.isophote_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.width(), img.height(), 0, GL_BGRA,
               GL_UNSIGNED_BYTE, img.convertToFormat(QImage::Format_ARGB32).bits());

  QImage img2(":/environment.png");
  glGenTextures(1, &vis.environment_texture);
  glBindTexture(GL_TEXTURE_2D, vis.environment_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img2.width(), img2.height(), 0, GL_BGRA,
               GL_UNSIGNED_BYTE, img2.convertToFormat(QImage::Format_ARGB32).bits());

  glGenTextures(1, &vis.slicing_texture);
  glBindTexture(GL_TEXTURE_1D, vis.slicing_texture);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  static const unsigned char slicing_img[] = { 0b11111111, 0b00011100 };
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 2, 0, GL_RGB, GL_UNSIGNED_BYTE_3_3_2, &slicing_img);
}

void Viewer::draw() {
  for (auto o : objects)
    o->draw(vis);

  if (axes.shown) {
    using qglviewer::Vec;
    const auto &p = Vec(axes.position);
    glColor3d(1.0, 0.0, 0.0);
    drawArrow(p, p + Vec(axes.size, 0.0, 0.0), axes.size / 50.0);
    glColor3d(0.0, 1.0, 0.0);
    drawArrow(p, p + Vec(0.0, axes.size, 0.0), axes.size / 50.0);
    glColor3d(0.0, 0.0, 1.0);
    drawArrow(p, p + Vec(0.0, 0.0, axes.size), axes.size / 50.0);
  }
}

void Viewer::drawWithNames() {
  for (size_t i = 0; i < objects.size(); ++i) {
    if (!axes.shown)
      glPushName(i);
    objects[i]->drawWithNames();
    if (!axes.shown)
      glPopName();
  }

  if (axes.shown) {
    using qglviewer::Vec;
    const auto &p = Vec(axes.position);
    glPushName(0);
    drawArrow(p, p + Vec(axes.size, 0.0, 0.0), axes.size / 50.0);
    glPopName();
    glPushName(1);
    drawArrow(p, p + Vec(0.0, axes.size, 0.0), axes.size / 50.0);
    glPopName();
    glPushName(2);
    drawArrow(p, p + Vec(0.0, 0.0, axes.size), axes.size / 50.0);
    glPopName();
  }
}

void Viewer::postSelection(const QPoint &p) {
  // int sel = selectedName();
  // if (sel == -1) {
  //   axes.shown = false;
  //   return;
  // }

  // if (axes.shown) {
  //   axes.selected_axis = sel;
  //   bool found;
  //   axes.grabbed_pos = camera()->pointUnderPixel(p, found);
  //   axes.original_pos = axes.position;
  //   if (!found)
  //     axes.shown = false;
  //   return;
  // }

  // selected_vertex = sel;
  // if (model_type == ModelType::MESH)
  //   axes.position = Vec(mesh.point(MyMesh::VertexHandle(sel)).data());
  // if (model_type == ModelType::BEZIER_SURFACE)
  //   axes.position = control_points[sel];
  // double depth = camera()->projectedCoordinatesOf(axes.position)[2];
  // Vec q1 = camera()->unprojectedCoordinatesOf(Vec(0.0, 0.0, depth));
  // Vec q2 = camera()->unprojectedCoordinatesOf(Vec(width(), height(), depth));
  // axes.size = (q1 - q2).norm() / 10.0;
  // axes.shown = true;
  // axes.selected_axis = -1;
}

void Viewer::keyPressEvent(QKeyEvent *e) {
  if (e->modifiers() == Qt::NoModifier)
    switch (e->key()) {
    case Qt::Key_R:
      for (auto o : objects)
        o->reload();
      update();
      break;
    case Qt::Key_O:
      if (camera()->type() == qglviewer::Camera::PERSPECTIVE)
        camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);
      else
        camera()->setType(qglviewer::Camera::PERSPECTIVE);
      update();
      break;
    case Qt::Key_P:
      vis.type = VisType::PLAIN;
      update();
      break;
    case Qt::Key_M:
      vis.type = VisType::MEAN;
      update();
      break;
    case Qt::Key_L:
      vis.type = VisType::SLICING;
      update();
      break;
    case Qt::Key_I:
      vis.type = VisType::ISOPHOTES;
      vis.current_isophote_texture = vis.isophote_texture;
      update();
      break;
    case Qt::Key_E:
      vis.type = VisType::ISOPHOTES;
      vis.current_isophote_texture = vis.environment_texture;
      update();
      break;
    case Qt::Key_C:
      vis.show_control_points = !vis.show_control_points;
      update();
      break;
    case Qt::Key_S:
      vis.show_solid = !vis.show_solid;
      update();
      break;
    case Qt::Key_W:
      vis.show_wireframe = !vis.show_wireframe;
      update();
      break;
    default:
      QGLViewer::keyPressEvent(e);
    }
  else if (e->modifiers() == Qt::KeypadModifier)
    switch (e->key()) {
    case Qt::Key_Plus:
      vis.slicing_scaling *= 2;
      update();
      break;
    case Qt::Key_Minus:
      vis.slicing_scaling /= 2;
      update();
      break;
    case Qt::Key_Asterisk:
      vis.slicing_dir = Vector(static_cast<double *>(camera()->viewDirection()));
      update();
      break;
    } else
    QGLViewer::keyPressEvent(e);
}

void Viewer::mouseMoveEvent(QMouseEvent *e) {
  return QGLViewer::mouseMoveEvent(e);
  // if (!axes.shown ||
  //     (axes.selected_axis < 0 && !(e->modifiers() & Qt::ControlModifier)) ||
  //     !(e->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier)) ||
  //     !(e->buttons() & Qt::LeftButton))
  //   return QGLViewer::mouseMoveEvent(e);

  // if (e->modifiers() & Qt::ControlModifier) {
  //   // move in screen plane
  //   double depth = camera()->projectedCoordinatesOf(axes.position)[2];
  //   axes.position = camera()->unprojectedCoordinatesOf(Vec(e->pos().x(), e->pos().y(), depth));
  // } else {
  //   Vec from, dir, axis(axes.selected_axis == 0, axes.selected_axis == 1, axes.selected_axis == 2);
  //   camera()->convertClickToLine(e->pos(), from, dir);
  //   auto p = intersectLines(axes.grabbed_pos, axis, from, dir);
  //   float d = (p - axes.grabbed_pos) * axis;
  //   axes.position[axes.selected_axis] = axes.original_pos[axes.selected_axis] + d;
  // }

  // if (model_type == ModelType::MESH)
  //   mesh.set_point(MyMesh::VertexHandle(selected_vertex),
  //                  Vector(static_cast<double *>(axes.position)));
  // if (model_type == ModelType::BEZIER_SURFACE)
  //   control_points[selected_vertex] = axes.position;
  // updateMesh();
  // update();
}

QString Viewer::helpString() const {
  QString text("<h2>Geometry Framework</h2>"
               "<p>This is a minimal framework for 3D mesh manipulation, which can be "
               "extended and used as a base for various geometry-relatedprojects.</p>"
               "<p>The following hotkeys are available:</p>"
               "<ul>"
               "<li>&nbsp;R: Reload model</li>"
               "<li>&nbsp;O: Toggle orthographic projection</li>"
               "<li>&nbsp;P: Set plain map (no coloring)</li>"
               "<li>&nbsp;M: Set mean curvature map</li>"
               "<li>&nbsp;L: Set slicing map<ul>"
               "<li>&nbsp;+: Increase slicing density</li>"
               "<li>&nbsp;-: Decrease slicing density</li>"
               "<li>&nbsp;*: Set slicing direction to view</li></ul></li>"
               "<li>&nbsp;I: Set isophote line map</li>"
               "<li>&nbsp;E: Set environment texture</li>"
               "<li>&nbsp;C: Toggle control polygon visualization</li>"
               "<li>&nbsp;S: Toggle solid (filled polygon) visualization</li>"
               "<li>&nbsp;W: Toggle wireframe visualization</li>"
               "</ul>"
               "<p>There is also a simple selection and movement interface, enabled "
               "only when the wireframe/controlnet is displayed: a mesh vertex can be selected "
               "by shift-clicking, and it can be moved by shift-dragging one of the "
               "displayed axes. Pressing ctrl enables movement in the screen plane.</p>"
               "<p>Note that libQGLViewer is furnished with a lot of useful features, "
               "such as storing/loading view positions, or saving screenshots. "
               "OpenMesh also has a nice collection of tools for mesh manipulation: "
               "decimation, subdivision, smoothing, etc. These can provide "
               "good comparisons to the methods you implement.</p>"
               "<p>This software can be used as a sample GUI base for handling "
               "parametric or procedural surfaces, as well. The power of "
               "Qt and libQGLViewer makes it easy to set up a prototype application. "
               "Feel free to modify and explore!</p>"
               "<p align=\"right\">Peter Salvi</p>");
  return text;
}

void Viewer::updateMeanMinMax() {
  std::vector<double> mean;
  for (auto o : objects) {
    if (!o->valid())
      continue;
    const auto &mesh = o->baseMesh();
    for (auto v : mesh.vertices())
      mean.push_back(mesh.data(v).mean);
  }

  size_t n = mean.size();
  if (n < 3)
    return;

  std::sort(mean.begin(), mean.end());
  size_t k = (double)n * vis.cutoff_ratio;
  vis.mean_min = std::min(mean[k ? k-1 : 0], 0.0);
  vis.mean_max = std::max(mean[k ? n-k : n-1], 0.0);
}

void Viewer::setupCamera() {
  double large = std::numeric_limits<double>::max();
  Vector box_min(large, large, large), box_max(-large, -large, -large);
  for (auto o : objects) {
    const auto &mesh = o->baseMesh();
    for (auto v : mesh.vertices()) {
      box_min.minimize(mesh.point(v));
      box_max.maximize(mesh.point(v));
    }
  }
  using qglviewer::Vec;
  camera()->setSceneBoundingBox(Vec(box_min.data()), Vec(box_max.data()));
  camera()->showEntireScene();

  vis.slicing_scaling = 20 / (box_max - box_min).max();

  setSelectedName(-1);
  axes.shown = false;

  update();
}
