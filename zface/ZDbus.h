#ifndef ZDBUS_H
#define ZDBUS_H

#include <QObject>
#include <QTimer>
#include <QtDBus/QtDBus>

struct SoundFileInfo
{
    int openStatus;
    int channels;
    int duration;
    int sampleSize;
    int sampleRate;
    QString tags;
};


class ZDbus : public QObject
{
Q_OBJECT
public:
    explicit ZDbus(QObject *parent = 0);

    void startAliveTimer(int _interval);

    bool setParameter(const QString & _category, const QString & _name, int _value);
    bool getParameter(const QString & _category, const QString & _name, int * _value);

    void sendRotaryEvent(const QString & _event, const QString & _action);
    void sendPlayEvent(const QString & _event);
    void sendRecordKey();
    bool sendOpenFileRequest(const QString & _fileName, SoundFileInfo * _info);

signals:

    // FIXME: это для теста. Переделать параметры в int.
    void gainChanged(const QString _gain, const QString _value);
    void paramChanged(const QString _param, const QString _value);
    void playStateChanged(int _state);
    void playPositionChanged(int _position);
    void recDurationChanged(int _duration);
    void messageForUser(unsigned int _code, int _type);


public slots:

private slots:

    void sendAlive();

    void receiveGain(const QString, const QString _gain, const QString _value);
    void receiveParam(const QString, const QString _param, const QString _value);
    void receivePlayState(int _state);
    void receivePlayPosition(int _position);
    void receiveRecDuration(int _duration);
    void receiveUserNotify(unsigned int _code);
    void receiveUserError(unsigned int _code);

private:
    QTimer * aliveTimer;

    static QDBusConnection bus;
};

extern ZDbus * zdbus;

#endif // ZDBUS_H
