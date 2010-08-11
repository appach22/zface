#include "ZKbdDbusHandler.h"
#include <QDebug>

ZKbdDbusHandler::ZKbdDbusHandler() :
    bus(QDBusConnection::systemBus())
{
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "UpKey", this, SLOT(UpKey(unsigned int)));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "DownKey", this, SLOT(DownKey(unsigned int)));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "LeftKey", this, SLOT(LeftKey(unsigned int)));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "RightKey", this, SLOT(RightKey(unsigned int)));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "EnterKey", this, SLOT(EnterKey(unsigned int)));
    bus.connect("", "", "com.speechpro.CursorKeysEvents", "EscapeKey", this, SLOT(EscapeKey(unsigned int)));
}


void ZKbdDbusHandler::UpKey(unsigned int _isPressed)
{
    processKeyEvent(0xFFFF, Qt::Key_Up, Qt::NoModifier, _isPressed, false);
}
void ZKbdDbusHandler::DownKey(unsigned int _isPressed)
{
    processKeyEvent(0xFFFF, Qt::Key_Down, Qt::NoModifier, _isPressed, false);
}
void ZKbdDbusHandler::LeftKey(unsigned int _isPressed)
{
    processKeyEvent(0xFFFF, Qt::Key_Left, Qt::NoModifier, _isPressed, false);
}
void ZKbdDbusHandler::RightKey(unsigned int _isPressed)
{
    processKeyEvent(0xFFFF, Qt::Key_Right, Qt::NoModifier, _isPressed, false);
}
void ZKbdDbusHandler::EnterKey(unsigned int _isPressed)
{
    processKeyEvent(0xFFFF, Qt::Key_Select, Qt::NoModifier, _isPressed, false);
}
void ZKbdDbusHandler::EscapeKey(unsigned int _isPressed)
{
    processKeyEvent(0xFFFF, Qt::Key_Escape, Qt::NoModifier, _isPressed, false);
}
