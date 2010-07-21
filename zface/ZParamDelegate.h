#ifndef ZPARAMDELEGATE_H
#define ZPARAMDELEGATE_H

#include <QStyledItemDelegate>

class ZParamDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
    explicit ZParamDelegate(QObject *parent = 0);

    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

signals:

public slots:

};

#endif // ZPARAMDELEGATE_H
