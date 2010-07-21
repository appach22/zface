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

    void setData(ZSelectParameter * _data);
    void setData(ZValueParameter * _data);
    ZParameter * getData();
    QString getValue();

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
    QProgressBar * progress;
    SettingType type;
    int cachedValue;
    ZParameter * data;
    //ZSelectParameter selectData;
    //ZValueParameter valueData;
};

#endif // ZSETTINGWIDGET_H
