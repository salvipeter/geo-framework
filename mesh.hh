#pragma once

#include "object.hh"

class Mesh : public Object {
public:
  Mesh(std::string filename);
  virtual ~Mesh();
  virtual void drawWithNames() const override;
  virtual void postSelection(int selected) override;
  virtual void update() override;
  virtual void reload() override;
};
