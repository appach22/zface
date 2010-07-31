#include <QtGui/QApplication>
#include "mainwindow.h"
#include "zfacedecoration.h"

#if defined(Q_WS_QWS)
#include <QWSServer>
#include <QtPlugin>
Q_IMPORT_PLUGIN(zkbd)
#endif


#if defined(Q_WS_QWS)
class ZKeyboardFilter : public QWSServer::KeyboardFilter
{
public:
    ZKeyboardFilter()
        :QWSServer::KeyboardFilter()
    { }

    bool filter(int, int, int, bool, bool)
    {
        mainWindow->restartInactivityTimers();
        return false;
    }

    MainWindow * mainWindow;
};
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
    a.setNavigationMode(Qt::NavigationModeKeypadDirectional/*NavigationModeKeypadTabOrder*/);
#endif
    MainWindow w;
#if defined(Q_WS_QWS)
    ZKeyboardFilter * filter = new ZKeyboardFilter();
    filter->mainWindow = &w;
    QWSServer::addKeyboardFilter(filter);
#endif
    w.show();
    return a.exec();
}
