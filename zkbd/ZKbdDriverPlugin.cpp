#include <qconfig.h>
#if defined(Q_WS_QWS)

#include "ZKbdDriverPlugin.h"
#include "ZKbdHandler.h"
#include "ZKbdDbusHandler.h"

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
    return QStringList() << "zkbd" << "zkbd-dbus";
}

QWSKeyboardHandler * ZKbdDriverPlugin::create(const QString & driver, const QString &)
{
    if (driver.toLower() == "zkbd") {
        return new ZKbdHandler();
    }
    else if (driver.toLower() == "zkbd-dbus") {
        return new ZKbdDbusHandler();
    }
    return 0;
}

Q_EXPORT_PLUGIN2(zkbd, ZKbdDriverPlugin)

#endif
