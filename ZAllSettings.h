#ifndef ZALLSETTINGS_H
#define ZALLSETTINGS_H

#include "ZParameter.h"
#include "ZSettingsNode.h"

#include <QWidget>
#include <QtXml/QXmlStreamReader>

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

    static QMap<QString, ZParameter> allParameters;

    static QList<ZSettingsTree> settingsTree;*/

    static void loadAllSettings(const QString & _xmlFileName, QWidget * _paramContainer, ZSettingsNode ** _settingsRoot);

private:

    static QWidget * paramContainer;
    static void getSetting(QXmlStreamReader & _xml, ZSettingsNode * _parentNode);
    static void getParameter(QXmlStreamReader & _xml, ZSettingsNode * _parentNode);

};

#endif // ZALLSETTINGS_H
