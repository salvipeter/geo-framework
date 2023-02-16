#include "object.hh"

#ifdef USE_JET_FITTING
#include "jet-wrapper.hh"
#endif

Object::Object(std::string filename) : filename(filename) {
}

Object::~Object() {
}

const BaseMesh &Object::baseMesh() const {
  return mesh;
}

void Object::draw(const Visualization &vis) const {
  glPolygonMode(GL_FRONT_AND_BACK,
                !vis.show_solid && vis.show_wireframe ? GL_LINE : GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);

  if (vis.show_solid || vis.show_wireframe) {
    if (vis.type == VisType::PLAIN)
      glColor3d(1.0, 1.0, 1.0);
    else if (vis.type == VisType::ISOPHOTES) {
      glBindTexture(GL_TEXTURE_2D, vis.current_isophote_texture);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glEnable(GL_TEXTURE_2D);
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
      glEnable(GL_TEXTURE_GEN_S);
      glEnable(GL_TEXTURE_GEN_T);
    } else if (vis.type == VisType::SLICING) {
      glBindTexture(GL_TEXTURE_1D, vis.slicing_texture);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glEnable(GL_TEXTURE_1D);
    }
    for (auto f : mesh.faces()) {
      glBegin(GL_POLYGON);
      for (auto v : f.vertices()) {
        if (vis.type == VisType::MEAN)
          glColor3dv(vis.colorMap(vis.mean_min, vis.mean_max, mesh.data(v).mean).data());
        else if (vis.type == VisType::SLICING)
          glTexCoord1d(mesh.point(v) | vis.slicing_dir * vis.slicing_scaling);
        glNormal3dv(mesh.normal(v).data());
        glVertex3dv(mesh.point(v).data());
      }
      glEnd();
    }
    if (vis.type == VisType::ISOPHOTES) {
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_GEN_T);
      glDisable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    } else if (vis.type == VisType::SLICING)
      glDisable(GL_TEXTURE_1D);
  }

  if (vis.show_solid && vis.show_wireframe) {
    glPolygonMode(GL_FRONT, GL_LINE);
    glColor3d(0.0, 0.0, 0.0);
    glDisable(GL_LIGHTING);
    for (auto f : mesh.faces()) {
      glBegin(GL_POLYGON);
      for (auto v : f.vertices())
        glVertex3dv(mesh.point(v).data());
      glEnd();
    }
    glEnable(GL_LIGHTING);
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Object::update() {
  update(false, false);
}

void Object::update(bool own_normal, bool own_mean) {
  mesh.request_face_normals();
  mesh.request_vertex_normals();
  mesh.update_face_normals();

#ifdef USE_JET_FITTING

  if (own_normal && own_mean) {
    // no need for jet fitting
    for (auto v : mesh.vertices()) {
      mesh.set_normal(v, normal(v));
      mesh.data(v).mean = meanCurvature(v);
    }
    return;
  }

  mesh.update_vertex_normals();
  std::vector<Vector> points;
  for (auto v : mesh.vertices())
    points.push_back(mesh.point(v));
  auto nearest = JetWrapper::Nearest(points, 20);

  for (auto v : mesh.vertices()) {
    auto jet = JetWrapper::fit(mesh.point(v), nearest, 2);
    bool reverse = (mesh.normal(v) | jet.normal) > 0;
    if (own_normal)
      mesh.set_normal(v, normal(v));
    else
      mesh.set_normal(v, -jet.normal);
    if (own_mean)
      mesh.data(v).mean = meanCurvature(v);
    else
      mesh.data(v).mean = (jet.k_min + jet.k_max) / 2;
    if (reverse && !own_normal)
      mesh.set_normal(v, -mesh.normal(v));
    if (reverse && !own_mean)
      mesh.data(v).mean *= -1;
  }

#else // !USE_JET_FITTING

  std::ignore = std::tie(own_normal, own_mean);
  for (auto v : mesh.vertices()) {
    mesh.set_normal(v, normal(v));
    mesh.data(v).mean = meanCurvature(v);
  }

#endif // USE_JET_FITTING
}

Vector Object::normal(BaseMesh::VertexHandle vh) const {
  auto v = OpenMesh::make_smart(vh, &mesh);
  Vector n(0.0, 0.0, 0.0);
  for (auto h : v.incoming_halfedges()) {
    if (h.is_boundary())
      continue;
    auto in_vec  = mesh.calc_edge_vector(h);
    auto out_vec = mesh.calc_edge_vector(h.next());
    double w = in_vec.sqrnorm() * out_vec.sqrnorm();
    n += (in_vec % out_vec) / (w == 0.0 ? 1.0 : w);
  }
  double len = n.length();
  if (len != 0.0)
    n /= len;
  return n;
}

double Object::meanCurvature(BaseMesh::VertexHandle vh) const {
  auto v = OpenMesh::make_smart(vh, &mesh);

  // Compute triangle strip area
  double vertex_area = 0;
  for (auto f : v.faces())
    vertex_area += mesh.calc_sector_area(f.halfedge());
  vertex_area /= 3.0;

  // Compute mean values using dihedral angles
  double mean = 0;
  for (auto h : v.incoming_halfedges()) {
    auto vec = mesh.calc_edge_vector(h);
    double angle = mesh.calc_dihedral_angle(h); // signed; returns 0 at the boundary
    mean += angle * vec.norm();
  }
  return mean / (4 * vertex_area);
}

bool Object::valid() const {
  return mesh.n_vertices() > 0;
}
