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
    int id;
    QList<int> dependencies;
    //int value;
    ZParameter() {}
    ZParameter(QString _category, QString _name, QString _visualName)
        { category = _category; name = _name; visualName = _visualName; id = -1; }
    virtual ~ZParameter() {}
};

struct ZSelectParameter : public ZParameter
{
    QList<ZSelectParameterItem> items;
    ZSelectParameter() {}
    ZSelectParameter(QString _category, QString _name, QString _visualName, QList<ZSelectParameterItem> & _items)
        : ZParameter(_category, _name, _visualName)
        { items = _items;}
};

struct ZValueParameter : public ZParameter
{
    QPair<int, int> range;
    QString unit;
    ZValueParameter() {}
    ZValueParameter(QString _category, QString _name, QString _visualName, QPair<int, int> & _range, QString _unit)
        : ZParameter(_category, _name, _visualName)
        { range = _range; unit = _unit;}
};

#endif // ZPARAMETER_H
