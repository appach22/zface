#include <QtGui/QVBoxLayout>
#include <QtGui/QRadioButton>
#include <QCoreApplication>

#include "ZSettingWidget.h"
#include "ZDbus.h"

#include <QDebug>

extern QMap<QString, int> allValues;

ZSettingWidget::ZSettingWidget(QWidget *parent) :
    QWidget(parent)
{
    type = Unknown;
    hide();
}

void ZSettingWidget::setData(ZSelectParameter * _data)
{
    type = Select;
    //selectData = _data;
    data = _data;
    cachedValue = allValues[data->name];
    //QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QLabel * caption = new QLabel(this);
    caption->setText(data->visualName);
    caption->setAlignment(Qt::AlignCenter);
    caption->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    mainLayout->addWidget(caption);
    box = new QGroupBox(this);
    box->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mainLayout->addWidget(box);

    QVBoxLayout * boxLayout = new QVBoxLayout(box);
    boxLayout->setContentsMargins(4, 4, 4, 4);
    buttons = new QButtonGroup(box);

    for (int i = 0; i < dynamic_cast<ZSelectParameter *>(data)->items.count(); ++i)
    {
        QRadioButton * radio = new QRadioButton(box);
        radio->setText(dynamic_cast<ZSelectParameter *>(data)->items[i].name);
        buttons->addButton(radio, i);
        boxLayout->addWidget(radio);
        if (cachedValue == dynamic_cast<ZSelectParameter *>(data)->items[i].value)
            radio->setChecked(true);
        radio->installEventFilter(this);
    }
    boxLayout->addStretch(1);
}

void ZSettingWidget::setData(ZValueParameter * _data)
{
    type = Value;
    data = _data;
    cachedValue = allValues[data->name];
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QLabel * caption = new QLabel(this);
    caption->setText(data->visualName);
    caption->setAlignment(Qt::AlignCenter);
    caption->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    mainLayout->addWidget(caption);

    progressContainer = new QWidget(this);
    //progressContainer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    progressContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(progressContainer);

    QVBoxLayout * valueLayout = new QVBoxLayout(progressContainer);
    //valueLayout->setSizeConstraint(QLayout::SetNoConstraint);
    valueLayout->addStretch(1);

    progress = new QProgressBar(progressContainer);
    progress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    progress->setMinimum(dynamic_cast<ZValueParameter *>(data)->range.first);
    progress->setMaximum(dynamic_cast<ZValueParameter *>(data)->range.second);
    progress->setFormat(QString("%v ") + dynamic_cast<ZValueParameter *>(data)->unit);
    progress->setValue(allValues[data->name]);
    valueLayout->addWidget(progress, 0, Qt::AlignCenter);
    valueLayout->addStretch(1);
    qDebug() << progress->width() << " " << ((QWidget*)progress->parent())->width();
}

void ZSettingWidget::setData(ZValueParameter * _data, ZCustomWidget * _customProcessor)
{
    type = Custom;
    customProcessor = _customProcessor;
    data = _data;
    QGridLayout * mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(_customProcessor, 0, 0);
}

ZParameter * ZSettingWidget::getData()
{
    return data;
}


void ZSettingWidget::keyPressEvent(QKeyEvent * event)
{
    int value;
    switch (event->key())
    {
        case Qt::Key_Select :
            if (type == Custom)
                break;
            // Получаем новое значение параметра
            if (type == Select)
            {
                for (int i = 0; i < dynamic_cast<ZSelectParameter *>(data)->items.count(); ++i)
                    if (buttons->button(i)->hasFocus())
                        value = dynamic_cast<ZSelectParameter *>(data)->items[i].value;
            }
            else if (type == Value)
                value = progress->value();

            // Сохраняем значение параметра
            if (zdbus->setParameter(data->category, data->name, value))
            {
                // Если значение успешно сохранено - обновляем Gui
                if (type == Select)
                {
                    for (int i = 0; i < dynamic_cast<ZSelectParameter *>(data)->items.count(); ++i)
                        if (buttons->button(i)->hasFocus())
                            buttons->button(i)->setChecked(true);
                }
                cachedValue = allValues[data->name] = value;
            }
            else
                qDebug() << "Error setting parameter: " << data->category << data->name << value;
            break;
        case Qt::Key_Down :
            if (type == Value)
            {
                if (cachedValue > dynamic_cast<ZValueParameter *>(data)->range.first)
                    progress->setValue(--cachedValue);
            }
            break;
        case Qt::Key_Up :
            if (type == Value)
            {
                if (cachedValue < dynamic_cast<ZValueParameter *>(data)->range.second)
                    progress->setValue(++cachedValue);
            }
            break;
        case Qt::Key_Left :
            if (type == Select)
                buttons->button(0)->setFocus();
            break;
        case Qt::Key_Right :
            if (type == Select)
            {
                int count = buttons->buttons().count();
                buttons->button(count - 1)->setFocus();
            }
            break;
    }
    QWidget::keyPressEvent(event);
}

void ZSettingWidget::showEvent(QShowEvent *)
{
    cachedValue = allValues[data->name];
    if (type == Select)
    {
        for (int i = 0; i < dynamic_cast<ZSelectParameter *>(data)->items.count(); ++i)
            if (cachedValue == dynamic_cast<ZSelectParameter *>(data)->items[i].value)
            {
                buttons->buttons()[i]->setChecked(true);
                buttons->buttons()[i]->setFocus();
            }
        box->setEditFocus(true);
    }
    else if (type == Value)
    {
        progress->setValue(cachedValue);
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

QString ZSettingWidget::getValue()
{
    if (type == Select)
    {
        ZSelectParameter * param = dynamic_cast<ZSelectParameter *>(data);
        cachedValue = allValues[param->name];
        for (int i = 0; i < param->items.count(); ++i)
            if (cachedValue == param->items[i].value)
                return param->items[i].name;
    }
    else if (type == Value)
        return QString("%1 ").arg(allValues[data->name]) + dynamic_cast<ZValueParameter *>(data)->unit;
    else if (type == Custom)
        return customProcessor->getValue();

    return "";
}
