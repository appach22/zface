#include "ZSettingsModel.h"
#include "ZParameter.h"
#include "ZSettingWidget.h"
#include <QDebug>

ZSettingsModel::ZSettingsModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootNode = 0;
}

ZSettingsModel::~ZSettingsModel()
{
    delete *rootNode;
}

void ZSettingsModel::setRootNode(ZSettingsNode ** _node)
{
    //delete rootNode;
    rootNode = _node;
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
        return *rootNode;
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

//bool addNode(const QString & _name, ZSettingsNode::Type _type, QWidget * _widget);

QString ZSettingsModel::getValueByName(const QString & _name)
{
    valueFound = false;
    valueByName(_name, *rootNode, GetValue);
    if (valueFound)
        return foundValue;
    else
        return "";
}

void ZSettingsModel::setValueByName(const QString & _name, int _value)
{
    valueFound = false;
    if (rootNode)
        valueByName(_name, *rootNode, SetValue, _value);
}

void ZSettingsModel::setNodeEnabled(const QString & _name, bool _enabled)
{
    if (rootNode)
        valueByName(_name, *rootNode, SetEnabled, static_cast<int>(_enabled));
}


void ZSettingsModel::valueByName(const QString & _name, ZSettingsNode * _root, NodeActions _action, int _value)
{
    if (valueFound)
        return;
    if (!_root)
        return;

//    if ()
    if (!_root->children.count())
    {
        if (_root->widget)
        {
            ZParameter * param = dynamic_cast<ZSettingWidget *>(_root->widget)->getData();
            if (param->name == _name)
            {
                if (_action == SetValue)
                    dynamic_cast<ZSettingWidget *>(_root->widget)->setValue(_value);
                else if (_action == GetValue)
                    foundValue = dynamic_cast<ZSettingWidget *>(_root->widget)->getValue();
                valueFound = true;
                return;
            }
        }
    }
    else
        for (int i = 0; i < _root->children.count(); ++i)
            valueByName(_name, _root->children[i], _action, _value);
}

