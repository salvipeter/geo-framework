#include <OpenMesh/Core/IO/MeshIO.hh>

#include "mesh.hh"

Mesh::Mesh(std::string filename) : Object(filename) {
  reload();
}

Mesh::~Mesh() {
}

void Mesh::drawWithNames(const Visualization &vis) const {
  if (!vis.show_wireframe)
    return;
  for (auto v : mesh.vertices()) {
    glPushName(v.idx());
    glRasterPos3dv(mesh.point(v).data());
    glPopName();
  }
}

Vector Mesh::postSelection(int selected) {
  return mesh.point(BaseMesh::VertexHandle(selected));
}

void Mesh::movement(int selected, const Vector &pos) {
  mesh.set_point(BaseMesh::VertexHandle(selected), pos);
}

void Mesh::update() {
  Object::update();
}

bool Mesh::reload() {
  if (!OpenMesh::IO::read_mesh(mesh, filename))
    return false;
  update();
  return true;
}
