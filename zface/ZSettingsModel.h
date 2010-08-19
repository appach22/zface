#ifndef ZSETTINGSMODEL_H
#define ZSETTINGSMODEL_H

#include <QAbstractItemModel>

#include "ZSettingsNode.h"

class ZSettingsModel : public QAbstractItemModel
{

    enum NodeActions
    {
        None = 0,
        GetValue,
        SetValue,
        SetEnabled
    };

public:

    explicit ZSettingsModel(QObject *parent = 0);
    ~ZSettingsModel();

    void setRootNode(ZSettingsNode ** _node);

    QModelIndex index(int row, int column, const QModelIndex & parent) const;
    QModelIndex parent(const QModelIndex & child) const;

    int rowCount(const QModelIndex & parent) const;
    int columnCount(const QModelIndex & parent) const;

    QVariant data(const QModelIndex & index, int role) const;

    QString getValueByName(const QString & _name);
    void setValueByName(const QString & _name, int _value);
    //void setNodeEnabled(const QString & _name, bool _enabled);

private:
    ZSettingsNode * nodeFromIndex(const QModelIndex & index) const;
    // TODO: возможно, не нужен
    ZSettingsNode ** rootNode;

    bool valueFound;
    QString foundValue;
    void valueByName(const QString & _name, ZSettingsNode * _root, NodeActions _action, int _value = 0);
};

#endif // ZSETTINGSMODEL_H
