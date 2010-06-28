#ifndef ZALLSETTINGS_H
#define ZALLSETTINGS_H

#include "ZParameter.h"
#include "ZSettingsNode.h"

#include <QWidget>
#include <QtXml/QXmlStreamReader>
#include <QMap>

/*struct ZSettingsTree
{
    QString name;
    ZSettingsNode::Type type;
    QString path;
    QList <ZSettingsTree> children;
};*/

class ZAllSettings
{
public:
/*    ZAllSettings();

    static QList<ZSettingsTree> settingsTree;*/

    static void loadAllSettings(const QString & _xmlFileName, QWidget * _paramContainer, const QString & _rootTag, ZSettingsNode ** _settingsRoot);

    static void setParameter(const QString & _name, int _value);

private:

    static QWidget * paramContainer;
    //static ZParameter * findParameter(ZSettingsNode * _rootNode, const QString & _category, const QString & _name);
    static void getSetting(QXmlStreamReader & _xml, ZSettingsNode * _parentNode);
    static void getParameter(QXmlStreamReader & _xml, ZSettingsNode * _parentNode);

};

#endif // ZALLSETTINGS_H
