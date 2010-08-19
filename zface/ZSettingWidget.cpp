#include <QtGui/QVBoxLayout>
#include <QCoreApplication>
#include <QStylePainter>
#include <QMessageBox>

#include "ZSettingWidget.h"
#include "ZParamDelegate.h"
#include "ZDbus.h"

#include <QDebug>

extern QMap<QString, int> allValues;


ZSelectButton::ZSelectButton(QWidget *parent) :
    QRadioButton(parent)
{
}

void ZSelectButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionButton opt;
    QRadioButton::initStyleOption(&opt);
    if (opt.state & QStyle::State_HasFocus)
    {
        opt.state &= ~QStyle::State_HasFocus;
        opt.state |= QStyle::State_Selected;
        //opt.state |= QStyle::State_MouseOver;
    }
    p.drawControl(QStyle::CE_RadioButton, opt);
}

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
    scroll = new QScrollArea(this);
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->setWidgetResizable(true);
    mainLayout->addWidget(scroll);

    QWidget * contentWidget = new QWidget(scroll);
    contentWidget->setObjectName("contentWidget");
    QVBoxLayout * boxLayout = new QVBoxLayout(contentWidget);
    boxLayout->setContentsMargins(4, 4, 4, 4);
    buttons = new QButtonGroup(contentWidget);

    for (int i = 0; i < dynamic_cast<ZSelectParameter *>(data)->items.count(); ++i)
    {
        ZSelectButton * radio = new ZSelectButton(contentWidget);
        radio->setText(dynamic_cast<ZSelectParameter *>(data)->items[i].name);
        buttons->addButton(radio, i);
        boxLayout->addWidget(radio);
        if (cachedValue == dynamic_cast<ZSelectParameter *>(data)->items[i].value)
            radio->setChecked(true);
        radio->installEventFilter(this);
    }
    boxLayout->addStretch(1);
    scroll->setWidget(contentWidget);
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
    //qDebug() << progress->width() << " " << ((QWidget*)progress->parent())->width();
}

void ZSettingWidget::setData(ZParameter * _data, ZCustomWidget * _customProcessor)
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

            // Проверяем сохраняемое значение
            if (!checkValue(data->name, value))
                return;

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
        case Qt::Key_Left :
            if (type == Select)
                buttons->button(0)->setFocus();
            else if (type == Value)
            {
                ZValueParameter * param = dynamic_cast<ZValueParameter *>(data);
                if (progress->value() - param->step >= param->range.first)
                    progress->setValue(progress->value() - param->step);
            }
            break;
        case Qt::Key_Right :
            if (type == Select)
            {
                int count = buttons->buttons().count();
                buttons->button(count - 1)->setFocus();
            }
            else if (type == Value)
            {
                ZValueParameter * param = dynamic_cast<ZValueParameter *>(data);
                if (progress->value() + param->step <= param->range.second)
                    progress->setValue(progress->value() + param->step);
            }
            break;
    }
    QWidget::keyPressEvent(event);
}

void ZSettingWidget::showEvent(QShowEvent *)
{
    //cachedValue = allValues[data->name];
    if (type == Select)
    {
        for (int i = 0; i < dynamic_cast<ZSelectParameter *>(data)->items.count(); ++i)
            if (cachedValue == dynamic_cast<ZSelectParameter *>(data)->items[i].value)
            {
                buttons->buttons()[i]->setChecked(true);
                buttons->buttons()[i]->setFocus();
            }
//        scroll->setEditFocus(true);
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
        //cachedValue = allValues[param->name];
        for (int i = 0; i < param->items.count(); ++i)
            if (cachedValue == param->items[i].value)
                return param->items[i].name;
    }
    else if (type == Value)
        return QString("%1 ").arg(/*allValues[data->name]*/cachedValue) + dynamic_cast<ZValueParameter *>(data)->unit;
    else if (type == Custom)
        return customProcessor->getValue();

    return "";
}

void ZSettingWidget::setValue(int _value)
{
    allValues[data->name] = _value;
    if (type != Custom)
        cachedValue = _value;
    else
        customProcessor->setValue(_value);
}

bool ZSettingWidget::checkValue(const QString & _param, int _value)
{
    if (_param == "Recorder.Acoustic.Start_threshold")
    {
        if (_value <= allValues["Recorder.Acoustic.Stop_threshold"])
        {
            QMessageBox::warning(this, "", trUtf8("Порог старта должен быть больше порога стопа."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
    }
    else if (_param == "Recorder.Acoustic.Stop_threshold")
    {
        if (_value >= allValues["Recorder.Acoustic.Start_threshold"])
        {
            QMessageBox::warning(this, "", trUtf8("Порог стопа должен быть меньше порога старта."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
    }
    else if (_param == "Recorder.Sample_size")
    {
        if (_value == 1 && allValues["Recorder.Compression"] == 1)
        {
            QMessageBox::warning(this, "", trUtf8("Разрядность 24 бита не поддерживается для сжатия μ-закон."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
    }
    else if (_param == "Recorder.Compression")
    {
        if (_value == 1 && allValues["Recorder.Sample_size"] == 1)
        {
            QMessageBox::warning(this, "", trUtf8("Сжатие μ-закон не поддерживается для разрядности 24 бита."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
    }
    else if (_param == "Broadband_filter.Enabled")
    {
        if (_value == 1 && allValues["ReferenceNoise_filter.Enabled"] == 1)
        {
            QMessageBox::warning(this, "", trUtf8("Сначала выключите Стерео фильтр."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
        else if (_value == 1 && allValues["Voice_changer_filter.Enabled"] == 1)
        {
            QMessageBox::warning(this, "", trUtf8("Сначала выключите Морфер."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }

    }
    else if (_param == "ReferenceNoise_filter.Enabled")
    {
        if (_value == 1 && allValues["Broadband_filter.Enabled"] == 1)
        {
            QMessageBox::warning(this, "", trUtf8("Сначала выключите Широкополосный фильтр."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
        else if (_value == 1 && allValues["Voice_changer_filter.Enabled"] == 1)
        {
            QMessageBox::warning(this, "", trUtf8("Сначала выключите Морфер."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }

    }
    else if (_param == "Declicker_filter.Enabled"     ||
             _param == "EQ_filter.Enabled"            ||
             _param == "HarmonicReject_filter.Enabled"||
             _param == "LevelEnhancement_filter.Enabled")
    {
        qDebug() << _value << allValues["Voice_changer_filter.Enabled"];
        if (_value == 1 && allValues["Voice_changer_filter.Enabled"] == 1)
        {
            QMessageBox::warning(this, "", trUtf8("Сначала выключите Морфер."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
    }
    else if (_param == "Voice_changer_filter.Enabled")
    {
        if (_value == 1)
            if (allValues["Broadband_filter.Enabled"]       == 1 ||
                allValues["Declicker_filter.Enabled"]       == 1 ||
                allValues["EQ_filter.Enabled"]              == 1 ||
                allValues["HarmonicReject_filter.Enabled"]  == 1 ||
                allValues["LevelEnhancement_filter.Enabled"]== 1 ||
                allValues["ReferenceNoise_filter.Enabled"]  == 1)
        {
            QMessageBox::warning(this, "", trUtf8("Сначала выключите все фильтры."), QMessageBox::Ok, QMessageBox::Ok);
            return false;
        }
    }
    return true;
}

