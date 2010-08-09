#ifndef ZKBDDBUSHANDLER_H
#define ZKBDDBUSHANDLER_H

#include <QObject>
#include <QWSKeyboardHandler>
#include <QtDBus/QtDBus>


class ZKbdDbusHandler : public QObject, public QWSKeyboardHandler
{
Q_OBJECT

    QDBusConnection bus;

public:

    ZKbdDbusHandler();

signals:

public slots:

private slots:
    void UpKey();
    void DownKey();
    void LeftKey();
    void RightKey();
    void EnterKey();
    void EscapeKey();

};

#endif // ZKBDDBUSHANDLER_H
