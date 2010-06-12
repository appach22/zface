#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
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
            radio->setFocus();
        }
    }
    boxLayout->addStretch(1);
}

void ZSettingWidget::setData(const ZValueParameter & _data)
{

}

void ZSettingWidget::keyPressEvent(QKeyEvent * event)
{
    QString path;
    int value;
    switch (event->key())
    {
        case Qt::Key_Return :
            path = selectData.path;
            if (type == Select)
                value = selectData.items[buttons->checkedId()].value;
            else if (type == Value)
                ;
            //ZDbus::setParameter(path, value);
            hide();
            break;
        case Qt::Key_Escape :
            hide();
            break;
    }
    QWidget::keyPressEvent(event);
}


