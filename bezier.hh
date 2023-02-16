#pragma once

#include "object.hh"

class Bezier : public Object {
public:
  virtual void drawWithNames() override;
  virtual void postSelection(int selected) override;
  virtual void update() override;
private:
  size_t degree[2];
  std::vector<Vector> control_points;
};
