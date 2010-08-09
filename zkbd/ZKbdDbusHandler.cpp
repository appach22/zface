#include "ZKbdDbusHandler.h"
#include <QDebug>

ZKbdDbusHandler::ZKbdDbusHandler() :
    bus(QDBusConnection::systemBus())
{
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "UpKey", this, SLOT(UpKey()));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "DownKey", this, SLOT(DownKey()));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "LeftKey", this, SLOT(LeftKey()));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "RightKey", this, SLOT(RightKey()));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "EnterKey", this, SLOT(EnterKey()));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "EscapeKey", this, SLOT(EscapeKey()));
}


void ZKbdDbusHandler::UpKey()
{
    processKeyEvent(0xFFFF, Qt::Key_Up, Qt::NoModifier, true, false);
}
void ZKbdDbusHandler::DownKey()
{
    processKeyEvent(0xFFFF, Qt::Key_Down, Qt::NoModifier, true, false);
}
void ZKbdDbusHandler::LeftKey()
{
    processKeyEvent(0xFFFF, Qt::Key_Left, Qt::NoModifier, true, false);
}
void ZKbdDbusHandler::RightKey()
{
    processKeyEvent(0xFFFF, Qt::Key_Right, Qt::NoModifier, true, false);
}
void ZKbdDbusHandler::EnterKey()
{
    processKeyEvent(0xFFFF, Qt::Key_Select, Qt::NoModifier, true, false);
}
void ZKbdDbusHandler::EscapeKey()
{
    processKeyEvent(0xFFFF, Qt::Key_Escape, Qt::NoModifier, true, false);
}
