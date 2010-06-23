#ifndef ZDBUS_H
#define ZDBUS_H

#include <QObject>

class ZDbus : public QObject
{
Q_OBJECT
public:
    explicit ZDbus(QObject *parent = 0);

    static bool setParameter(QString _category, QString _name, int _value);

signals:

public slots:

private slots:

};

#endif // ZDBUS_H
