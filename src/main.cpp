#include "MainWindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QIcon>
#include <QStyleFactory>
#include <QWidget>

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  app.setStyle(QStyleFactory::create("Fusion"));

  QCoreApplication::setOrganizationName("tyleruploads");
  QCoreApplication::setApplicationName("github.com/tyleruploads");
  QCoreApplication::setOrganizationDomain("MDEZ++");
  QCoreApplication::setApplicationVersion(APP_VERSION);

  QApplication::setWindowIcon(QIcon(":/media/docs/mdez-icon.svg"));

  MainWindow window;
  window.resize(800, 800);
  window.setWindowTitle("Untitled | MDEZ++");

  window.show();

  return app.exec();
}
