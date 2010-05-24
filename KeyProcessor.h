#ifndef KEYPROCESSOR_H
#define KEYPROCESSOR_H

#include <QtGui/QKeyEvent>

//#include "mainwindow.h"
class MainWindow;

namespace Ui
{
    class MainWindow;
}

namespace zface
{

    class KeyProcessor
    {
    public:

        KeyProcessor(MainWindow * _wnd);
        void ProcessKey(QKeyEvent * event);

    private:
        Ui::MainWindow * m_ui;
        MainWindow * m_wnd;

    };

} // namespace zface

#endif // KEYPROCESSOR_H
