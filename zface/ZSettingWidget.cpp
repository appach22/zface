#include <QtGui/QVBoxLayout>
#include <QtGui/QRadioButton>
#include <QCoreApplication>

#include "ZSettingWidget.h"
#include "ZDbus.h"

#include <QDebug>


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
    box = new QGroupBox(this);
    box->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mainLayout->addWidget(box);

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
            radio->setChecked(true);
        radio->installEventFilter(this);
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

    progressContainer = new QWidget(this);
    progressContainer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mainLayout->addWidget(progressContainer);

    QVBoxLayout * valueLayout = new QVBoxLayout(progressContainer);
    valueLayout->addStretch(1);

    valueLabel = new QLabel(progressContainer);
    valueLabel->setAlignment(Qt::AlignCenter);
    valueLabel->setText(QString("%1 ").arg(valueData.value) + valueData.unit);
    valueLayout->addWidget(valueLabel, 0, Qt::AlignCenter);

    progress = new QProgressBar(progressContainer);
    progress->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    progress->setMinimum(valueData.range.first);
    progress->setMaximum(valueData.range.second);
    progress->setFormat(QString("%v ") + valueData.unit);
    progress->setValue(valueData.value);
    valueLayout->addWidget(progress, 0, Qt::AlignCenter);
    valueLayout->addStretch(1);
}

void ZSettingWidget::keyPressEvent(QKeyEvent * event)
{
    QString category, name;
    int value;
    switch (event->key())
    {
        case Qt::Key_Select :
            // Получаем новое значение параметра
            category = selectData.category;
            name = selectData.name;
            if (type == Select)
            {
                for (int i = 0; i < selectData.items.count(); ++i)
                    if (buttons->button(i)->hasFocus())
                        value = selectData.items[i].value;
            }
            else if (type == Value)
                value = progress->value();

            // Сохраняем значение параметра
            if (ZDbus::setParameter(category, name, value))
            {
                // Если значение успешно сохранено - обновляем Gui
                if (type == Select)
                {
                    for (int i = 0; i < selectData.items.count(); ++i)
                        if (buttons->button(i)->hasFocus())
                            buttons->button(i)->setChecked(true);
                    selectData.value = value;
                }
                else if (type == Value)
                    valueData.value = value;
            }
            else
                qDebug() << "Error setting parameter: " << category << name << value;
            break;
        case Qt::Key_Down :
            if (type == Value)
            {
                if (valueData.value > valueData.range.first)
                    progress->setValue(--valueData.value);
                valueLabel->setText(QString("%1 ").arg(valueData.value) + valueData.unit);
                progress->repaint();
            }
            break;
        case Qt::Key_Up :
            if (type == Value)
            {
                if (valueData.value < valueData.range.second)
                    progress->setValue(++valueData.value);
                valueLabel->setText(QString("%1 ").arg(valueData.value) + valueData.unit);
                progress->repaint();
            }
            break;
    }
    QWidget::keyPressEvent(event);
}

void ZSettingWidget::showEvent(QShowEvent * event)
{
    if (type == Select)
    {
        for (int i = 0; i < selectData.items.count(); ++i)
            if (selectData.value == selectData.items[i].value)
            {
                buttons->buttons()[i]->setChecked(true);
                buttons->buttons()[i]->setFocus();
            }
        box->setEditFocus(true);
    }
    else if (type == Value)
    {
        progress->setFocus();
        progressContainer->setEditFocus(true);
    }
}

bool ZSettingWidget::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Select)
        {
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    return false;
}
