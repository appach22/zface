#ifndef ZSETTINGSMODEL_H
#define ZSETTINGSMODEL_H

#include <QAbstractItemModel>

#include "ZSettingsNode.h"

class ZSettingsModel : public QAbstractItemModel
{
public:
    explicit ZSettingsModel(QObject *parent = 0);
    ~ZSettingsModel();

    void setRootNode(ZSettingsNode * _node);

    QModelIndex index(int row, int column, const QModelIndex & parent) const;
    QModelIndex parent(const QModelIndex & child) const;

    int rowCount(const QModelIndex & parent) const;
    int columnCount(const QModelIndex & parent) const;

    QVariant data(const QModelIndex & index, int role) const;

private:
    ZSettingsNode * nodeFromIndex(const QModelIndex & index) const;
    // TODO: возможно, не нужен
    ZSettingsNode * rootNode;

};

#endif // ZSETTINGSMODEL_H
