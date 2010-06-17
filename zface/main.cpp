#include <QtGui/QApplication>
#include "mainwindow.h"

#if defined(Q_WS_QWS)
#include <QtPlugin>
Q_IMPORT_PLUGIN(zkbd)
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
