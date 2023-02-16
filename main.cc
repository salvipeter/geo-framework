#include <QtWidgets/QApplication>

#include "window.hh"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  Window window(&app);
  window.show();
  return app.exec();
}
