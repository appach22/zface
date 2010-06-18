#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>

#include "ZSettingWidget.h"

ZSettingWidget::ZSettingWidget(QWidget *parent) :
    QWidget(parent)
{
    type = Unknown;
    hide();
}

void ZSettingWidget::setData(const ZSelectParameter & _data)
{
    type = Select;
    selectData = _data;
    //QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QLabel * caption = new QLabel(this);
    caption->setText(_data.visualName);
    caption->setAlignment(Qt::AlignCenter);
    caption->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    mainLayout->addWidget(caption);
    QGroupBox * box = new QGroupBox(this);
    box->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mainLayout->addWidget(box);
    //box->setFocus();

    QVBoxLayout * boxLayout = new QVBoxLayout(box);
    boxLayout->setContentsMargins(4, 4, 4, 4);
    buttons = new QButtonGroup(box);

    for (int i = 0; i < _data.items.count(); ++i)
    {
        QRadioButton * radio = new QRadioButton(box);
        radio->setText(selectData.items[i].name);
        buttons->addButton(radio, i);
        boxLayout->addWidget(radio);
        if (selectData.value == selectData.items[i].value)
        {
            radio->setChecked(true);
            radio->setEditFocus(true);
        }
    }
    boxLayout->addStretch(1);
}

void ZSettingWidget::setData(const ZValueParameter & _data)
{
    type = Value;
    valueData = _data;

    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QLabel * caption = new QLabel(this);
    caption->setText(_data.visualName);
    caption->setAlignment(Qt::AlignCenter);
    caption->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    mainLayout->addWidget(caption);

    QWidget * widget = new QWidget(this);
    widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mainLayout->addWidget(widget);

    QVBoxLayout * valueLayout = new QVBoxLayout(widget);
    valueLayout->addStretch(1);

    valueLabel = new QLabel(widget);
    valueLabel->setAlignment(Qt::AlignCenter);
    valueLabel->setText(QString("%1 ").arg(valueData.value) + valueData.unit);
    valueLayout->addWidget(valueLabel, 0, Qt::AlignCenter);

    progress = new QProgressBar(widget);
    progress->setMinimum(valueData.range.first);
    progress->setMaximum(valueData.range.second);
    progress->setValue(valueData.value);
    valueLayout->addWidget(progress, 0, Qt::AlignCenter);
    valueLayout->addStretch(1);
}

void ZSettingWidget::keyPressEvent(QKeyEvent * event)
{
    QString path;
    int value;
    switch (event->key())
    {
        case Qt::Key_Select :
            path = selectData.path;
            if (type == Select)
                value = selectData.items[buttons->checkedId()].value;
            else if (type == Value)
                value = progress->value();
            //ZDbus::setParameter(path, value);
            hide();
            break;
        case Qt::Key_Escape :
            hide();
            break;
        case Qt::Key_Left :
            if (type == Value)
            {
                progress->setValue(++valueData.value);
                valueLabel->setText(QString("%1 ").arg(valueData.value) + valueData.unit);
            }
        case Qt::Key_Right :
            if (type == Value)
            {
                progress->setValue(++valueData.value);
                valueLabel->setText(QString("%1 ").arg(valueData.value) + valueData.unit);
            }
            break;
    }
    QWidget::keyPressEvent(event);
}


