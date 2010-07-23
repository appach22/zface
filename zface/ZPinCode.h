#ifndef ZPINCODE_H
#define ZPINCODE_H

#include <QWidget>
#include <QtGui/QKeyEvent>

#include "ZCustomWidget.h"
#include "ZDbus.h"

namespace Ui {
    class ZPinCode;
}

class ZPinCode : public ZCustomWidget {
    Q_OBJECT
public:
    ZPinCode(QWidget *parent = 0);
    ~ZPinCode();

    QString getValue();

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::ZPinCode *ui;

    ZDbus zdbus;
    int cachedValue;
};

#endif // ZPINCODE_H
