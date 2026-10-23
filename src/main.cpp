#include "MainWindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QIcon>
#include <QStyleFactory>
#include <QWidget>

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  QGuiApplication::setDesktopFileName("io.github.tyleruploads.mdez");

  app.setStyle(QStyleFactory::create("Fusion"));

  QCoreApplication::setOrganizationName("tyleruploads");
  QCoreApplication::setApplicationName("mdez");
  QCoreApplication::setOrganizationDomain("github.com/tyleruploads");
  QCoreApplication::setApplicationVersion(APP_VERSION);
 
  MainWindow window;
  window.resize(800, 800);
  window.setWindowTitle("Untitled | MDEZ++");

  QApplication::setWindowIcon(QIcon(":/data/io.github.tyleruploads.mdez.png"));
  window.setWindowIcon(QIcon(":/data/io.github.tyleruploads.mdez.png"));

  window.show();

  return app.exec();
}
