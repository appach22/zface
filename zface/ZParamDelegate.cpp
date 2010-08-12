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
    //newOption.state &= ~QStyle::State_Enabled;
    //newOption.state = 0;
    //QStyledItemDelegate::paint(painter, newOption, index);

    if (index.data(Qt::DisplayRole).isValid())
    {
//        if (option.state & QStyle::State_Selected)
//            painter->fillRect(option.rect, option.palette.highlight());
//        else if (index.row() % 2)
//            painter->fillRect(option.rect, option.palette.base());
//        else
//            painter->fillRect(option.rect, option.palette.alternateBase());

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
//        ZSettingsNode * node = static_cast<ZSettingsNode *>(index.internalPointer());
//        if (node->type == ZSettingsNode::Leaf)
//        {
//            QString value = dynamic_cast<ZSettingWidget *>(node->widget)->getValue();
//            if (!value.isEmpty())
//            {
//                QSize s = QStyledItemDelegate::sizeHint(option, index);
//                painter->save();
//                QTextDocument doc;
//                doc.setHtml("<div>" + index.data(Qt::DisplayRole).toString() + "</div><div style=\"font-style: italic;\">" + value + "</div>");
//                QAbstractTextDocumentLayout::PaintContext context;
//                doc.setPageSize(s);
//                doc.setDocumentMargin(0);
//                painter->translate(option.rect.x(), option.rect.y());
//                doc.documentLayout()->draw(painter, context);
//                painter->restore();
//            }
//            else
//                QStyledItemDelegate::paint(painter, newOption, index);
//        }
//        else
//            QStyledItemDelegate::paint(painter, newOption, index);
//   }
   else
       QStyledItemDelegate::paint(painter, newOption, index);
}
