#ifndef ZDBUS_H
#define ZDBUS_H

#include <QObject>
#include <QTimer>
#include <QtDBus/QtDBus>

class ZDbus : public QObject
{
Q_OBJECT
public:
    explicit ZDbus(QObject *parent = 0);

    void startAliveTimer(int _interval);

    bool setParameter(const QString & _category, const QString & _name, int _value);
    bool getParameter(const QString & _category, const QString & _name, int * _value);

    void sendRotaryEvent(const QString & _event, const QString & _action);

signals:

    // FIXME: это для теста. Переделать параметры в int.
    void gainChanged(const QString _gain, const QString _value);
    void paramChanged(const QString _param, const QString _value);

public slots:

private slots:

    void sendAlive();

    void receiveGain(const QString, const QString _gain, const QString _value);
    void receiveParam(const QString, const QString _param, const QString _value);

private:
    QTimer * aliveTimer;

    static QDBusConnection bus;
};

extern ZDbus * zdbus;

#endif // ZDBUS_H
