// -*- mode: c++ -*-
#pragma once

#ifdef USE_JET_FITTING

#include <OpenMesh/Core/Geometry/VectorT.hh>

namespace JetWrapper {

using Vector3D = OpenMesh::VectorT<double,3>;
using Point3D = Vector3D;
using PointVector = std::vector<Point3D>;

// radius = 0                     means   KNN search
// radius > 0 and neighbors = 0   means   unlimited search in a fuzzy sphere
// radius > 0 and neighbors > 0   means   fuzzy sphere search with limited # of elements
class Nearest {
public:
  Nearest(const PointVector &points, size_t neighbors = 20, double radius = 0);
  ~Nearest();
  PointVector operator()(const Point3D &p) const;

private:
  const PointVector &points;
  size_t neighbors;
  double radius;
  void *tree; // to avoid a bunch of CGAL includes
};

struct JetData {
  Vector3D normal;       // Unit normal vector
  Vector3D d_min, d_max; // Principal curvature directions
  double k_min, k_max;   // Principal curvature values
};

JetData fit(const Point3D &p, const Nearest &nearest, size_t degree = 2);

}

#endif // USE_JET_FITTING
