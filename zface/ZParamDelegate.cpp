#include "ZParamDelegate.h"
#include "ZSettingsNode.h"
#include "ZSettingWidget.h"

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>

#include <QDebug>

ZParamDelegate::ZParamDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QSize ZParamDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    ZSettingsNode * node = static_cast<ZSettingsNode *>(index.internalPointer());
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    //qDebug() << s;
    if (node->type == ZSettingsNode::Leaf && !dynamic_cast<ZSettingWidget *>(node->widget)->getValue().isEmpty())
        return QSize(s.width(), s.height() * 3);
    else
        return QSize(s.width(), s.height() * 2);

    return s;
}

void ZParamDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyleOptionViewItem newOption = option;
    newOption.state &= ~QStyle::State_HasFocus;

    if (index.data(Qt::DisplayRole).isValid())
    {
        painter->fillRect(option.rect, option.palette.base());
        QRect insideRect = option.rect.adjusted(2, 2, -2, -2);
        QLinearGradient gradient(insideRect.topLeft(), insideRect.bottomLeft());
        gradient.setSpread(QGradient::PadSpread);
        if (option.state & QStyle::State_Selected)
            gradient.setStops(QGradientStops() << QGradientStop(0, QColor(102, 143, 189)) << QGradientStop(1, QColor(33, 105, 190)));
        else
            gradient.setStops(QGradientStops() << QGradientStop(0, QColor(159, 175, 191)) << QGradientStop(1, QColor(232, 232, 234)));
        QBrush * brush = new QBrush(gradient);
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawRoundedRect(QRectF(insideRect), 5, 5);

        painter->setFont(option.font);
        if (option.state & QStyle::State_Selected)
            painter->setPen(option.palette.highlightedText().color());
        else
            painter->setPen(option.palette.text().color());
        ZSettingsNode * node = static_cast<ZSettingsNode *>(index.internalPointer());
        QString name = index.data(Qt::DisplayRole).toString();
        QString value;
        if (node->type == ZSettingsNode::Leaf)
            value = dynamic_cast<ZSettingWidget *>(node->widget)->getValue();
        if (value.isEmpty())
            painter->drawText(insideRect.left() + 4, insideRect.top() + insideRect.height() / 2 + option.fontMetrics.ascent() / 2, name);
        else
        {
            painter->drawText(insideRect.left() + 4, insideRect.top() + insideRect.height() / 2 - 2, name);
            if (option.state & QStyle::State_Selected)
                painter->setPen(QColor(160, 160, 160));
            else
                painter->setPen(QColor(100, 100, 100));
            painter->drawText(insideRect.left() + 4, insideRect.top() + insideRect.height() / 2 + option.fontMetrics.ascent() + 2, value);
        }
    }
   else
       QStyledItemDelegate::paint(painter, newOption, index);
}

ZFileOpsDelegate::ZFileOpsDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QSize ZFileOpsDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    return QSize(s.width(), s.height() * 1.5);
}

void ZFileOpsDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyleOptionViewItem newOption = option;
    newOption.state &= ~QStyle::State_HasFocus;
    QStyledItemDelegate::paint(painter, newOption, index);
}
