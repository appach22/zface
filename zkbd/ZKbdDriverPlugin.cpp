#include "ZKbdDriverPlugin.h"
#include "ZKbdHandler.h"

#include <QDebug>

ZKbdDriverPlugin::ZKbdDriverPlugin(QObject *parent) :
    QKbdDriverPlugin (parent)
{
}

ZKbdDriverPlugin::~ZKbdDriverPlugin()
{

}

QStringList ZKbdDriverPlugin::keys() const
{
    return QStringList() << "zkbd";
}

QWSKeyboardHandler * ZKbdDriverPlugin::create(const QString & driver, const QString &)
{
    if (driver.toLower() == "zkbd") {
        qDebug() << "Before call ZKbdHandler()";
        return new ZKbdHandler();
    }
    return 0;
}

Q_EXPORT_PLUGIN2(zkbd, ZKbdDriverPlugin)
