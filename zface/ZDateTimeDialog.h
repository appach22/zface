#ifndef ZDATETIMEDIALOG_H
#define ZDATETIMEDIALOG_H

#include <QWidget>
#include <QtGui/QKeyEvent>

#include "ZCustomWidget.h"

namespace Ui {
    class ZDateTimeDialog;
}

class ZDateTimeDialog : public ZCustomWidget {
    Q_OBJECT
public:
    ZDateTimeDialog(QWidget *parent = 0);
    ~ZDateTimeDialog();

    QString getValue();

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::ZDateTimeDialog *ui;

};

#endif // ZDATETIMEDIALOG_H
