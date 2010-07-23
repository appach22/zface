#include "ZAllSettings.h"
#include "ZSettingWidget.h"
#include "ZDbus.h"
#include "ZDateTimeDialog.h"
#include "ZStartTimerDialog.h"
#include "ZTimerDurationDialog.h"
#include "ZPinCode.h"

#include <QFile>

#include <QDebug>
#include <QDir>

QWidget * ZAllSettings::paramContainer;
QMap<QString, int> allValues;

void ZAllSettings::loadAllSettings(const QString & _xmlFileName, QWidget * _paramContainer, const QString & _rootTag, ZSettingsNode ** _settingsRoot)
{
    //qDebug() << QDir::currentPath();

    QFile * file = new QFile(_xmlFileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    paramContainer = _paramContainer;

    QXmlStreamReader xml(file);
    *_settingsRoot = new ZSettingsNode(0, QObject::tr(""), ZSettingsNode::Node);

    bool rootTagDetected = false;
    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        if (xml.isStartElement() && xml.name().toString() == _rootTag)
            rootTagDetected = true;
        else if (xml.isStartElement() && rootTagDetected && xml.name().toString().toLower() == "setting")
            getSetting(xml, *_settingsRoot);
        else if (xml.isEndElement() && xml.name().toString() == _rootTag)
            break;
    }

    if (xml.hasError())
        qDebug() << "XML error!";
}

void ZAllSettings::setParameter(const QString & _name, int _value)
{
    allValues[_name] = _value;
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

    QString category = attrs.value("category").toString();
    QString name = attrs.value("name").toString();
    int value;
    if (attrs.value("type").toString().toLower() == "value")
    {
        QPair<int, int> range = QPair<int, int>(attrs.value("min").toString().toInt(), attrs.value("max").toString().toInt());
        ZValueParameter * param = new ZValueParameter(category, name, _parentNode->name, range, attrs.value("unit").toString());
        ZSettingWidget * setting = new ZSettingWidget(paramContainer);
        setting->setData(param);
        _parentNode->SetWidget(setting);

        zdbus->getParameter(category, name, &value);
    }
    else if (attrs.value("type").toString().toLower() == "select")
    {
        ZSelectParameter * param = new ZSelectParameter();
        param->category = category;
        param->name = name;
        param->visualName = _parentNode->name;
        _xml.readNext();
        while (!_xml.atEnd() && !_xml.hasError())
        {
            if (_xml.isEndElement() && _xml.name().toString().toLower() == "param")
                break;
            else if (_xml.isStartElement() && _xml.name().toString().toLower() == "item")
            {
                attrs = _xml.attributes();
                param->items.append(ZSelectParameterItem(attrs.value("value").toString().toInt(), attrs.value("name").toString()));
            }
            _xml.readNext();
        }
        ZSettingWidget * setting = new ZSettingWidget(paramContainer);
        setting->setData(param);
        _parentNode->SetWidget(setting);

        zdbus->getParameter(category, name, &value);
    }
    else if (attrs.value("type").toString().toLower() == "custom")
    {
        ZValueParameter * param = new ZValueParameter(/*category, name, _parentNode->name, QPair<int, int>(), attrs.value("unit").toString()*/);
        ZSettingWidget * setting = new ZSettingWidget(paramContainer);
        ZCustomWidget * customWidget;
        if (name == "DateTime")
            customWidget = new ZDateTimeDialog(setting);
        else if (name == "Recorder.Timer.Start_time")
            customWidget = new ZStartTimerDialog(setting);
        else if (name == "Recorder.Timer.Duration")
            customWidget = new ZTimerDurationDialog(setting);
        else if (name == "Security.Protection.PIN")
            customWidget = new ZPinCode(setting);
        setting->setData(param, customWidget);
        _parentNode->SetWidget(setting);
    }
}
