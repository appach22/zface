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
        return QSize(s.width(), s.height() * 2);
    else
        return QSize(s.width(), s.height() * 1.5);

    return s;
}

void ZParamDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyleOptionViewItem newOption = option;
    newOption.state &= ~QStyle::State_HasFocus;
    //QStyledItemDelegate::paint(painter, newOption, index);

    if (index.data(Qt::DisplayRole).isValid())
    {
        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());
        else if (index.row() % 2)
            painter->fillRect(option.rect, option.palette.base());
        else
            painter->fillRect(option.rect, option.palette.alternateBase());

        ZSettingsNode * node = static_cast<ZSettingsNode *>(index.internalPointer());
        if (node->type == ZSettingsNode::Leaf)
        {
            QString value = dynamic_cast<ZSettingWidget *>(node->widget)->getValue();
            if (!value.isEmpty())
            {
                QSize s = QStyledItemDelegate::sizeHint(option, index);
                painter->save();
                QTextDocument doc;
                doc.setHtml("<div>" + index.data(Qt::DisplayRole).toString() + "</div><div style=\"font-style: italic;\">" + value + "</div>");
                QAbstractTextDocumentLayout::PaintContext context;
                doc.setPageSize(s);
                doc.setDocumentMargin(0);
                painter->translate(option.rect.x(), option.rect.y());
                doc.documentLayout()->draw(painter, context);
                painter->restore();
            }
            else
                QStyledItemDelegate::paint(painter, newOption, index);
        }
        else
            QStyledItemDelegate::paint(painter, newOption, index);
   }
   else
       QStyledItemDelegate::paint(painter, newOption, index);
}
