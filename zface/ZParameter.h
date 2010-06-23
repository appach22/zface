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
    QString category;
    QString name;
    QString visualName;
    int value;
    ZParameter() {}
    ZParameter(QString _category, QString _name, QString _visualName, int _value)
        { category = _category; name = _name; visualName = _visualName; value = _value;}
};

struct ZSelectParameter : public ZParameter
{
    QList<ZSelectParameterItem> items;
    ZSelectParameter() {}
    ZSelectParameter(QString _category, QString _name, QString _visualName, int _value, QList<ZSelectParameterItem> & _items)
        : ZParameter(_category, _name, _visualName, _value)
        { items = _items;}
};

struct ZValueParameter : public ZParameter
{
    QPair<int, int> range;
    QString unit;
    ZValueParameter() {}
    ZValueParameter(QString _category, QString _name, QString _visualName, int _value, QPair<int, int> & _range, QString _unit)
        : ZParameter(_category, _name, _visualName, _value)
        { range = _range; unit = _unit;}
};

#endif // ZPARAMETER_H
