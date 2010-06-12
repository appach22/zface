#include "ZAllSettings.h"
#include "ZSettingWidget.h"

#include <QFile>

#include <QDebug>
#include <QDir>

QWidget * ZAllSettings::paramContainer;

void ZAllSettings::loadAllSettings(const QString & _xmlFileName, QWidget * _paramContainer, ZSettingsNode ** _settingsRoot)
{
    qDebug() << QDir::currentPath();

    QFile * file = new QFile(_xmlFileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    paramContainer = _paramContainer;

    QXmlStreamReader xml(file);
    *_settingsRoot = new ZSettingsNode(0, QObject::tr(""), ZSettingsNode::Node);

    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        if (xml.isStartElement() && xml.name().toString().toLower() == "setting")
            getSetting(xml, *_settingsRoot);
    }

    if (xml.hasError())
        qDebug() << "XML error!";
}


void ZAllSettings::getSetting(QXmlStreamReader & _xml, ZSettingsNode * _parentNode)
{
    QXmlStreamAttributes attrs = _xml.attributes();
    ZSettingsNode::Type type = (attrs.value("type").toString().toLower() == "node")
                               ? ZSettingsNode::Node
                               : ZSettingsNode::Leaf;
    QString name = attrs.value("name").toString();
    ZSettingsNode * node = new ZSettingsNode(_parentNode, name, type);
    if (type == ZSettingsNode::Leaf)
    {
        while (!_xml.atEnd() && !_xml.hasError())
        {
            _xml.readNext();
            if (_xml.isStartElement() && _xml.name().toString().toLower() == "param")
                getParameter(_xml, node);
            else if (_xml.isEndElement() && _xml.name().toString().toLower() == "setting")
                break;
        }
    }
    else if (type == ZSettingsNode::Node)
    {
        while (!_xml.atEnd() && !_xml.hasError())
        {
            _xml.readNext();
            if (_xml.isStartElement() && _xml.name().toString().toLower() == "setting")
                getSetting(_xml, node);
            else if (_xml.isEndElement() && _xml.name().toString().toLower() == "setting")
                break;
        }
    }
}

void ZAllSettings::getParameter(QXmlStreamReader & _xml, ZSettingsNode * _parentNode)
{
    QXmlStreamAttributes attrs = _xml.attributes();

    QString path = attrs.value("path").toString();
    //int value = ZDbus::getParam(path);
    if (attrs.value("type").toString().toLower() == "value")
    {
        QPair<int, int> range = QPair<int, int>(attrs.value("min").toString().toInt(), attrs.value("max").toString().toInt());
        ZValueParameter param = ZValueParameter(path, _parentNode->name, /*value*/0, range, attrs.value("unit").toString());
        ZSettingWidget * setting = new ZSettingWidget(paramContainer);
        setting->setData(param);
        _parentNode->SetWidget(setting);
    }
    else if (attrs.value("type").toString().toLower() == "select")
    {
        ZSelectParameter param;
        param.path = path;
        param.value = /*value*/0;
        param.visualName = _parentNode->name;
        _xml.readNext();
        while (!_xml.atEnd() && !_xml.hasError())
        {
            if (_xml.isEndElement() && _xml.name().toString().toLower() == "param")
                break;
            else if (_xml.isStartElement() && _xml.name().toString().toLower() == "item")
            {
                attrs = _xml.attributes();
                param.items.append(ZSelectParameterItem(attrs.value("value").toString().toInt(), attrs.value("name").toString()));
            }
            _xml.readNext();
        }
        ZSettingWidget * setting = new ZSettingWidget(paramContainer);
        setting->setData(param);
        _parentNode->SetWidget(setting);
    }
}
