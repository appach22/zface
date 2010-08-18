#ifndef ZSTARTTIMERDIALOG_H
#define ZSTARTTIMERDIALOG_H

#include <QWidget>
#include <QtGui/QKeyEvent>

#include "ZCustomWidget.h"
#include "ZDbus.h"

namespace Ui {
    class ZDateTimeDialog;
}

class ZStartTimerDialog : public ZCustomWidget {
    Q_OBJECT
public:
    ZStartTimerDialog(QWidget *parent = 0, const QString & _header = "");
    ~ZStartTimerDialog();

    QString getValue();
    void setValue(int _value);

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::ZDateTimeDialog *ui;

    ZDbus zdbus;
    QDateTime cachedValue;
};

#endif // ZSTARTTIMERDIALOG_H
