#include "ZKeyboardButton.h"
#include <QStylePainter>
#include <QStyleOptionButton>

ZKeyboardButton::ZKeyboardButton(QWidget *parent) :
    QPushButton(parent)
{
}

void ZKeyboardButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    if (option.state & QStyle::State_HasFocus)
    {
        option.state &= ~QStyle::State_HasFocus;
        option.state |= QStyle::State_Selected;
    }
    p.drawControl(QStyle::CE_PushButton, option);
}
