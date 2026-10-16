#include "MainWindow.h"
#include <QCoreApplication>
#include <QApplication>
#include <QWidget>
#include <QIcon>


int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    QApplication::setWindowIcon(QIcon("media/docs/mdez-icon.svg"));

    MainWindow window;
    window.resize(800, 800);
    window.setWindowTitle("Untitled | MDEZ++"); 

    window.show();

    return app.exec();
}
