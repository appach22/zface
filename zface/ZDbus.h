#ifndef ZDBUS_H
#define ZDBUS_H

#include <QObject>
#include <QTimer>

class ZDbus : public QObject
{
Q_OBJECT
public:
    explicit ZDbus(QObject *parent = 0);

    void startAliveTimer(int _interval);

    static bool setParameter(QString _category, QString _name, int _value);

signals:

public slots:

private slots:

    void sendAlive();

private:
    QTimer * aliveTimer;

};

#endif // ZDBUS_H
