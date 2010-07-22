#ifndef ZCUSTOMWIDGET_H
#define ZCUSTOMWIDGET_H

#include <QWidget>

class ZCustomWidget : public QWidget
{
Q_OBJECT
public:
    explicit ZCustomWidget(QWidget *parent = 0);

    virtual QString getValue() = 0;

signals:

public slots:

};

#endif // ZCUSTOMWIDGET_H
