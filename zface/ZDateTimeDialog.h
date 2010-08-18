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
    ZDateTimeDialog(QWidget *parent = 0, const QString & _header = "");
    ~ZDateTimeDialog();

    QString getValue();
    void setValue(int _value);

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::ZDateTimeDialog *ui;

};

#endif // ZDATETIMEDIALOG_H
