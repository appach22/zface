#include <qconfig.h>
#if defined(Q_WS_QWS)

#ifndef ZKBDHANDLER_H
#define ZKBDHANDLER_H

#include <QWSKeyboardHandler>
#include <QSocketNotifier>
#include <linux/input.h>

class ZKbdHandler : public QObject, public QWSKeyboardHandler
{
Q_OBJECT

    enum { inputsCount = 5 };

public:
    ZKbdHandler();
    ~ZKbdHandler();

private:
    static const QString inputsNames[inputsCount];
    int inputsFds[inputsCount];
    QSocketNotifier * inputsNotifiers[inputsCount];
    input_event event;

signals:

private slots:
    void readKbdData(int _fd);

};

#endif // ZKBDHANDLER_H

#endif
