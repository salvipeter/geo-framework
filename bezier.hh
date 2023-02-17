#pragma once

#include "object.hh"

class Bezier : public Object {
public:
  Bezier(std::string filename);
  virtual ~Bezier();
  virtual void draw(const Visualization &vis) const override;
  virtual void drawWithNames(const Visualization &vis) const override;
  virtual Vector postSelection(int selected) override;
  virtual void movement(int selected, const Vector &pos) override;
  virtual void updateBaseMesh() override;
  virtual bool reload() override;
private:
  size_t degree[2];
  std::vector<Vector> control_points;
};
