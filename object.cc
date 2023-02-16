#include "object.hh"

#include <OpenMesh/Core/Mesh/SmartHandles.hh>

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
      for (auto v : mesh.fv_range(f))
        glVertex3dv(mesh.point(v).data());
      glEnd();
    }
    glEnable(GL_LIGHTING);
  }
}

void Object::drawWithNames() const {
}

void Object::postSelection(int selected) {
}

void Object::update() {
  mesh.request_face_normals();
  mesh.request_vertex_normals();
  mesh.update_face_normals();
  for (auto v : mesh.vertices()) {
    mesh.set_normal(v, normal(v));
    mesh.data(v).mean = meanCurvature(v);
  }
}

Vector Object::normal(BaseMesh::VertexHandle vh) const {
  // TODO: jet fitting version
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
  for (auto f : mesh.vf_range(v))
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
