#include <QtGui/QApplication>
#include "mainwindow.h"
#include "zfacedecoration.h"

#if defined(Q_WS_QWS)
#include <QWSServer>
#include <QtPlugin>
Q_IMPORT_PLUGIN(zkbd)
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if defined(Q_WS_QWS)
    QWSServer::setCursorVisible(false);
    ZFaceDecoration *decoration = new ZFaceDecoration();
    a.qwsSetDecoration(decoration);
#endif
#if defined(QT_KEYPAD_NAVIGATION)
    a.setNavigationMode(Qt::NavigationModeKeypadDirectional);
#endif
    MainWindow w;
    w.show();
    return a.exec();
}
