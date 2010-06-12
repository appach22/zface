#ifndef ZPARAMETER_H
#define ZPARAMETER_H

#include <QString>
#include <QList>
#include <QPair>

struct ZSelectParameterItem
{
    int value;
    QString name;
    ZSelectParameterItem(int _value, QString _name)
        {value = _value; name = _name;}
};

struct ZParameter
{
    QString path;
    QString visualName;
    int value;
    ZParameter() {}
    ZParameter(QString _path, QString _visualName, int _value)
        { path = _path; visualName = _visualName; value = _value;}
};

struct ZSelectParameter : public ZParameter
{
    QList<ZSelectParameterItem> items;
    ZSelectParameter() {}
    ZSelectParameter(QString _path, QString _visualName, int _value, QList<ZSelectParameterItem> & _items)
        : ZParameter(_path, _visualName, _value)
        { items = _items;}
};

struct ZValueParameter : public ZParameter
{
    QPair<int, int> range;
    QString unit;
    ZValueParameter() {}
    ZValueParameter(QString _path, QString _visualName, int _value, QPair<int, int> & _range, QString _unit)
        : ZParameter(_path, _visualName, _value)
        { range = _range; unit = _unit;}
};

#endif // ZPARAMETER_H
