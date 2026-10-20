#include "MainWindow.h"
#include <QCoreApplication>
#include <QApplication>
#include <QWidget>
#include <QIcon>


int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    QCoreApplication::setOrganizationName("tyleruploads");
    QCoreApplication::setOrganizationDomain("https://github.com/tyleruploads");
    QCoreApplication::setOrganizationDomain("MDEZ++");
    QCoreApplication::setApplicationVersion(APP_VERSION);

    QApplication::setWindowIcon(QIcon("media/docs/mdez-icon.svg"));

    MainWindow window;
    window.resize(800, 800);
    window.setWindowTitle("Untitled | MDEZ++"); 

    window.show();

    return app.exec();
}
