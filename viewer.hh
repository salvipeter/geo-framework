#pragma once

#include <QGLViewer/qglviewer.h>
#include "object.hh"

class Viewer : public QGLViewer {
  Q_OBJECT

public:
  explicit Viewer(QWidget *parent);

  double getCutoffRatio() const;
  void setCutoffRatio(double ratio);
  double getMeanMin() const;
  void setMeanMin(double min);
  double getMeanMax() const;
  void setMeanMax(double max);
  const double *getSlicingDir() const;
  void setSlicingDir(double x, double y, double z);
  double getSlicingScaling() const;
  void setSlicingScaling(double scaling);
  void deleteObjects();
  bool openMesh(const std::string &filename, bool update_view = true);
  bool openBezier(const std::string &filename, bool update_view = true);
  bool saveBezier(const std::string &filename);

signals:
  void startComputation(QString message);
  void midComputation(int percent);
  void endComputation();

protected:
  virtual void init() override;
  virtual void draw() override;
  virtual void drawWithNames() override;
  virtual void postSelection(const QPoint &p) override;
  virtual void keyPressEvent(QKeyEvent *e) override;
  virtual void mouseMoveEvent(QMouseEvent *e) override;
  virtual QString helpString() const override;

private:
  void updateMeanMinMax();
  void setupCamera();

  //////////////////////
  // Member variables //
  //////////////////////

  std::vector<std::shared_ptr<Object>> objects;
  Visualization vis;

  int selected_vertex;
  struct ModificationAxes {
    bool shown;
    float size;
    int selected_axis;
    Vector position, grabbed_pos, original_pos;
  } axes;
};
