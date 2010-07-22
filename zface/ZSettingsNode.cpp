#include "ZSettingsNode.h"
#include <QDebug>

ZSettingsNode::ZSettingsNode(ZSettingsNode * _parent, const QString & _name, Type _type, QWidget * _widget)
{
    name = _name;
    type = _type;
    parent = _parent;
    widget = _widget;
    if (parent)
        parent->children.append(this);
}

ZSettingsNode::~ZSettingsNode()
{
    qDeleteAll(children);
}

bool ZSettingsNode::SetWidget(QWidget * _widget)
{
    if (type != Leaf)
        return false;

    widget = _widget;
    return true;
}
