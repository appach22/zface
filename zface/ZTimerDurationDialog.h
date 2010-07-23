#ifndef ZTIMERDURATIONDIALOG_H
#define ZTIMERDURATIONDIALOG_H

#include <QWidget>
#include <QtGui/QKeyEvent>

#include "ZCustomWidget.h"
#include "ZDbus.h"

namespace Ui {
    class ZDateTimeDialog;
}

class ZTimerDurationDialog : public ZCustomWidget {
    Q_OBJECT
public:
    ZTimerDurationDialog(QWidget *parent = 0);
    ~ZTimerDurationDialog();

    QString getValue();

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::ZDateTimeDialog *ui;

    ZDbus zdbus;
    QTime cachedValue;
};

#endif // ZTIMERDURATIONDIALOG_H
