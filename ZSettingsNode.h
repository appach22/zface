#ifndef ZSETTINGSNODE_H
#define ZSETTINGSNODE_H

#include <QWidget>
#include <QString>
#include <QList>

struct ZSettingsNode
{
    enum Type {Node, Leaf};

    ZSettingsNode(ZSettingsNode * _parent, const QString & _name, Type _type = Node, QWidget * _widget = 0);
    ~ZSettingsNode();

    bool SetWidget(QWidget * _widget);

    ZSettingsNode * parent;
    QList<ZSettingsNode *> children;
    QString name;
    Type type;
    // TODO: убрать зависимость от GUI !!!!!!!!!!!1
    QWidget * widget;
};

#endif // ZSETTINGSNODE_H
