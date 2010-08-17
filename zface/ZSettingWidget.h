#ifndef ZSETTINGWIDGET_H
#define ZSETTINGWIDGET_H

#include "ZParameter.h"
#include "ZCustomWidget.h"

#include <QWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollArea>

class ZSelectButton : public QRadioButton
{
    Q_OBJECT

public:
    explicit ZSelectButton(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent * event);
};

class ZSettingWidget : public QWidget
{
Q_OBJECT

    enum SettingType {Unknown, Select, Value, Custom};

public:

    explicit ZSettingWidget(QWidget *parent = 0);

    void setData(ZSelectParameter * _data);
    void setData(ZValueParameter * _data);
    void setData(ZParameter * _data, ZCustomWidget * _customProcessor);
    ZParameter * getData();
    QString getValue();
    void setValue(int _value);

protected:
    void keyPressEvent(QKeyEvent * event);
    void showEvent(QShowEvent * event);
    bool eventFilter(QObject *obj, QEvent *event);

signals:

private slots:

private:

    QButtonGroup * buttons;
    QScrollArea * scroll;
    QWidget * progressContainer;
    QProgressBar * progress;
    SettingType type;
    int cachedValue;
    ZParameter * data;
    ZCustomWidget * customProcessor;
};

#endif // ZSETTINGWIDGET_H
