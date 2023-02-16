#ifdef USE_JET_FITTING

#include <boost/iterator/function_output_iterator.hpp>

#include <CGAL/Eigen_svd.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Fuzzy_sphere.h>
#include <CGAL/Monge_via_jet_fitting.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>

#include "jet-wrapper.hh"

namespace JetWrapper {

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_3 = Kernel::Point_3;
using Vector_3 = Kernel::Vector_3;
using Tree_traits = CGAL::Search_traits_3<Kernel>;
using Splitter = CGAL::Sliding_midpoint<Tree_traits>;
using Tree = CGAL::Kd_tree<Tree_traits, Splitter, CGAL::Tag_true>;
using Distance = CGAL::Euclidean_distance<Tree_traits>;
using Neighbor_search = CGAL::Orthogonal_k_neighbor_search<Tree_traits, Distance, Splitter, Tree>;
using Search_iterator = Neighbor_search::iterator;
using Sphere = CGAL::Fuzzy_sphere<Tree_traits>;

// Ugly conversion routines
static inline const Point_3 &conv(const Point3D &p) {
  return *reinterpret_cast<const Point_3 *>(&p);
}
static inline const Point3D &conv(const Point_3 &p) {
  return *reinterpret_cast<const Point3D *>(&p);
}
static inline const Vector3D &conv(const Vector_3 &p) {
  return *reinterpret_cast<const Vector3D *>(&p);
}
static inline const std::vector<Point_3> &conv(const PointVector &pv) {
  return *reinterpret_cast<const std::vector<Point_3> *>(&pv);
}

// Based on CGAL/Point_set_processing_3/internal/Neighbor_query.h

Nearest::Nearest(const PointVector &points, size_t neighbors, double radius)
  : points(points), neighbors(neighbors), radius(radius)
{
  const std::vector<Point_3> &pts = conv(points);
  auto t = new Tree(pts.begin(), pts.end());
  t->build();
  tree = reinterpret_cast<void *>(t);
}

Nearest::~Nearest() {
  delete reinterpret_cast<Tree *>(tree);
}

struct Maximum_points_reached_exception : public std::exception { };

PointVector Nearest::operator()(const Point3D &p) const {
  PointVector output;
  if (radius != 0) {
    // Radius search
    Sphere fs(conv(p), radius, 0);
    size_t k = neighbors > 0 ? neighbors : std::numeric_limits<size_t>::max();
    size_t count = 0;
    try {
      std::function<void(const Point_3 &)> limited_search =
        [&](const Point_3 &it) {
          output.push_back(conv(it));
          if (++count == k)
            throw Maximum_points_reached_exception();
        };
      auto output_iterator = boost::make_function_output_iterator(limited_search);
      reinterpret_cast<Tree *>(tree)->search(output_iterator, fs);
    } catch (const Maximum_points_reached_exception &) {
    }
  } else {
    // KNN search
    Distance distance;
    Neighbor_search search(*reinterpret_cast<Tree *>(tree), conv(p), neighbors + 1,
                           0, true, distance);
    Search_iterator search_iterator = search.begin();
    for (size_t i = 0; i < neighbors + 1; ++i) {
      if(search_iterator == search.end())
        break; // not enough points
      output.push_back(conv(search_iterator->first));
      search_iterator++;
    }
  }
  return output;
}

// Based on CGAL/jet_estimate_normals.h

using Monge_jet_fitting = CGAL::Monge_via_jet_fitting<Kernel,
                                                      CGAL::Simple_cartesian<double>,
                                                      CGAL::Eigen_svd>;
using Monge_form = Monge_jet_fitting::Monge_form;

JetData fit(const Point3D &p, const Nearest &nearest, size_t degree) {
  JetData result;
  auto samples = conv(nearest(p));
  Monge_jet_fitting monge_fit;
  auto monge_form = monge_fit(samples.begin(), samples.end(), degree, 2);
  result.normal = conv(monge_form.normal_direction());
  result.d_min = conv(monge_form.minimal_principal_direction());
  result.d_max = conv(monge_form.maximal_principal_direction());
  result.k_min = monge_form.principal_curvatures(1);
  result.k_max = monge_form.principal_curvatures(0);
  return result;
}

}

#endif // USE_JET_FITTING
