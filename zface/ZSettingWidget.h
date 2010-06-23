#ifndef ZSETTINGWIDGET_H
#define ZSETTINGWIDGET_H

#include "ZParameter.h"

#include <QWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QGroupBox>


class ZSettingWidget : public QWidget
{
Q_OBJECT

    enum SettingType {Unknown, Select, Value};

public:

    explicit ZSettingWidget(QWidget *parent = 0);

    void setData(const ZSelectParameter & _data);
    void setData(const ZValueParameter & _data);

protected:
    void keyPressEvent(QKeyEvent * event);
    void showEvent(QShowEvent * event);
    bool eventFilter(QObject *obj, QEvent *event);

signals:

private slots:

private:

    QButtonGroup * buttons;
    QGroupBox * box;
    QWidget * progressContainer;
    QLabel * valueLabel;
    QProgressBar * progress;
    SettingType type;
    ZSelectParameter selectData;
    ZValueParameter valueData;
};

#endif // ZSETTINGWIDGET_H
