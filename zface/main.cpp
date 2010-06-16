#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QtPlugin>

Q_IMPORT_PLUGIN(zkbd)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
