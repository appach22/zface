#include "ZDbus.h"

#include <QtDBus/QtDBus>


ZDbus::ZDbus(QObject *parent) :
    QObject(parent)
{
}

void ZDbus::startAliveTimer(int _interval)
{
    aliveTimer = new QTimer(this);
    connect(aliveTimer, SIGNAL(timeout()), this, SLOT(sendAlive()));
    aliveTimer->start(_interval);
}

void ZDbus::sendAlive()
{
    QDBusMessage aliveMessage = QDBusMessage::createSignal("/alive", "com.speechpro.HeartBeats", "zfaceAlive");
#ifdef Q_WS_QWS
    QDBusConnection::systemBus().send(aliveMessage);
#else
    QDBusConnection::sessionBus().send(aliveMessage);
#endif
}

bool ZDbus::setParameter(QString _category, QString _name, int _value)
{
    QDBusMessage method = QDBusMessage::createMethodCall("com.speechpro.zvar", "/", "com.speechpro.ParamChangeRequests", "ChangeParam");
    method << _category << _name << QString("%1").arg(_value);
#ifdef Q_WS_QWS
    QDBusMessage reply = QDBusConnection::systemBus().call(method, QDBus::Block, 3000);
#else
    QDBusMessage reply = QDBusConnection::sessionBus().call(method, QDBus::Block, 3000);
#endif
    if (reply.type() == QDBusMessage::ErrorMessage)
        return false;
    if (reply.arguments().count() < 2)
        return false;
    bool ok;
    int res;
    res = reply.arguments()[1].toInt(&ok);
    return (ok && !res);
}
