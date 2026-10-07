#include "MainWindow.h"
#include <QApplication>
#include <QWidget>


int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    MainWindow window;
    window.resize(800, 800);
    window.setWindowTitle("MDEZ++"); 

    window.show();

    return app.exec();
}
