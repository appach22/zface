#include "ZSettingsModel.h"
#include <QDebug>

ZSettingsModel::ZSettingsModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootNode = 0;
}

ZSettingsModel::~ZSettingsModel()
{
    delete rootNode;
}

void ZSettingsModel::setRootNode(ZSettingsNode * _node)
{
    //delete rootNode;
    rootNode = _node;
    qDebug() << "root " << rootNode->name;
    reset();
}

QModelIndex ZSettingsModel::index(int row, int column, const QModelIndex & parent) const
{
    if (!rootNode || row < 0 || column < 0)
        return QModelIndex();
    ZSettingsNode *parentNode = nodeFromIndex(parent);
    ZSettingsNode *childNode = parentNode->children.value(row);
    if (!childNode)
        return QModelIndex();
    return createIndex(row, column, childNode);
}

ZSettingsNode * ZSettingsModel::nodeFromIndex(const QModelIndex & index) const
{
    if (index.isValid())
        return static_cast<ZSettingsNode *>(index.internalPointer());
    else
        return rootNode;
}

int ZSettingsModel::rowCount(const QModelIndex & parent) const
{
    if (parent.column() > 0)
        return 0;
    ZSettingsNode * parentNode = nodeFromIndex(parent);
    if (!parentNode)
        return 0;
    return parentNode->children.count();
}

int ZSettingsModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QModelIndex ZSettingsModel::parent(const QModelIndex & child) const
{
    ZSettingsNode * node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();
    ZSettingsNode * parentNode = node->parent;
    if (!parentNode)
        return QModelIndex();
    ZSettingsNode * grandparentNode = parentNode->parent;
    if (!grandparentNode)
        return QModelIndex();

    int row = grandparentNode->children.indexOf(parentNode);
        return createIndex(row, 0, parentNode);
}

QVariant ZSettingsModel::data(const QModelIndex & index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    ZSettingsNode * node = nodeFromIndex(index);
    if (!node)
        return QVariant();
    if (index.column() == 0)
        return node->name;
    return QVariant();
}

bool addNode(const QString & _name, ZSettingsNode::Type _type, QWidget * _widget);
