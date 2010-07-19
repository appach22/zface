#include "ZDbus.h"

#ifdef Q_WS_QWS
    QDBusConnection ZDbus::bus = QDBusConnection::systemBus();
#else
    QDBusConnection ZDbus::bus = QDBusConnection::sessionBus();
#endif

ZDbus * zdbus = new ZDbus();

ZDbus::ZDbus(QObject *parent) :
    QObject(parent)
{
    bus.connect("", "", "com.speechpro.GainChangedEvents", "GainChanged", this, SLOT(receiveGain(QString, QString, QString)));
    bus.connect("", "", "com.speechpro.ParamChangedEvents", "ParamChanged", this, SLOT(receiveParam(QString, QString, QString)));
    bus.connect("", "", "com.speechpro.PlayStateEvents", "PlayStateEvent", this, SLOT(receivePlayState(int)));
    bus.connect("", "", "com.speechpro.PlayPositionEvents", "PositionChanged", this, SLOT(receivePlayPosition(int)));
    bus.connect("", "", "com.speechpro.RecDurationEvents", "RecDuration", this, SLOT(receiveRecDuration(int)));
    bus.connect("", "", "com.speechpro.MessageForUser", "Notify", this, SLOT(receiveUserNotify(unsigned int)));
    //bus.connect("", "", "com.speechpro.MessageForUser", "Error", this, SLOT(receiveUserError(int)));
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
    bus.send(aliveMessage);
}

bool ZDbus::setParameter(const QString & _category, const QString & _name, int _value)
{
    QDBusMessage method = QDBusMessage::createMethodCall("com.speechpro.zvar", "/", "com.speechpro.ParamChangeRequests", "ChangeParam");
    method << _category << _name << QString("%1").arg(_value);
    QDBusMessage reply = bus.call(method, QDBus::Block, 3000);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return false;
    if (reply.arguments().count() < 2)
        return false;
    bool ok;
    int res;
    res = reply.arguments()[1].toInt(&ok);
    return (ok && !res);
}

bool ZDbus::getParameter(const QString & _category, const QString & _name, int * _value)
{
    QDBusMessage method = QDBusMessage::createMethodCall("com.speechpro.zvar", "/", "com.speechpro.ParamsRequests", "RequestParam");
    method << _category << _name;
    QDBusMessage reply = bus.call(method, QDBus::Block, 3000);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return false;
    if (reply.arguments().count() < 1)
        return false;
    bool ok;
    *_value = reply.arguments()[0].toInt(&ok);
    if (ok)
        emit paramChanged(_name, reply.arguments()[0].toString());
    return ok;
}

void ZDbus::sendRotaryEvent(const QString & _event, const QString & _action)
{
    QDBusMessage rotaryMessage = QDBusMessage::createSignal("/", "com.speechpro.EncodersEvents", _event);
    rotaryMessage << _action;
    bus.send(rotaryMessage);
}

void ZDbus::sendPlayEvent(const QString & _event)
{
    QDBusMessage playMessage = QDBusMessage::createSignal("/", "com.speechpro.PlayControl", _event);
    bus.send(playMessage);
}


bool  ZDbus::sendOpenFileRequest(const QString & _fileName, SoundFileInfo * _info)
{
    QDBusMessage method = QDBusMessage::createMethodCall("com.speechpro.zplay", "/", "com.speechpro.PlayControl", "OpenFile");
    method << _fileName;
    QDBusMessage reply = bus.call(method, QDBus::Block, 3000);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return false;
    bool ok;
    _info->openStatus = reply.arguments()[0].toInt(&ok);
    if (!ok)
        return ok;
    if (reply.arguments().count() > 1)
    {
        _info->duration = reply.arguments()[1].toInt(&ok);
        if (!ok) return ok;
        _info->sampleSize = reply.arguments()[2].toInt(&ok);
        if (!ok) return ok;
        _info->channels = reply.arguments()[3].toInt(&ok);
        if (!ok) return ok;
        _info->sampleRate = reply.arguments()[4].toInt(&ok);
        if (!ok) return ok;
    }
    if (reply.arguments().count() > 5)
        _info->tags = reply.arguments()[5].toString();

    return true;
}

void ZDbus::receiveGain(const QString, const QString _gain, const QString _value)
{
    emit gainChanged(_gain, _value);
}

void ZDbus::receiveParam(const QString, const QString _param, const QString _value)
{
    emit paramChanged(_param, _value);
}

void ZDbus::receivePlayState(int _state)
{
    emit playStateChanged(_state);
}

void ZDbus::receivePlayPosition(int _position)
{
    emit playPositionChanged(_position);
}

void ZDbus::receiveRecDuration(int _duration)
{
    emit recDurationChanged(_duration);
}

void ZDbus::receiveUserNotify(unsigned int _code)
{
    emit messageForUser(_code, 0);
}

void ZDbus::receiveUserError(unsigned int _code)
{
    emit messageForUser(_code, 1);
}

