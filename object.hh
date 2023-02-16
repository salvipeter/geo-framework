#pragma once

#include "base-mesh.hh"
#include "visualization.hh"

class Object {
public:
  explicit Object(std::string filename);
  virtual ~Object();
  const BaseMesh &baseMesh() const;
  virtual void draw(const Visualization &vis) const;
  virtual void drawWithNames() const;
  virtual void postSelection(int selected);
  virtual void update();
  virtual Vector normal(BaseMesh::VertexHandle vh) const;
  virtual double meanCurvature(BaseMesh::VertexHandle vh) const;
  virtual void reload() = 0;
  bool valid() const;
protected:
  BaseMesh mesh;
  std::string filename;
};
