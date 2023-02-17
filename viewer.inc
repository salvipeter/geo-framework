// vim: set ft=cpp: -*- mode: c++ -*-

template <typename T>
bool Viewer::openObject(std::string filename) {
  auto surface = std::make_shared<T>(filename);
  if (!surface->valid())
    return false;
  objects.push_back(surface);
  updateMeanMinMax();
  setupCamera();
  return true;
}
