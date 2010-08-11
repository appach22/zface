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
    void UpKey(unsigned int);
    void DownKey(unsigned int);
    void LeftKey(unsigned int);
    void RightKey(unsigned int);
    void EnterKey(unsigned int);
    void EscapeKey(unsigned int);

};

#endif // ZKBDDBUSHANDLER_H
