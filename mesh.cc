#include <OpenMesh/Core/IO/MeshIO.hh>

#include "mesh.hh"

Mesh::Mesh(std::string filename) : Object(filename) {
  reload();
}

Mesh::~Mesh() {
}

void Mesh::drawWithNames() const {
}

void Mesh::postSelection(int selected) {
}

void Mesh::update() {
}

void Mesh::reload() {
  OpenMesh::IO::read_mesh(mesh, filename);
}
