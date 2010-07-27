#include "ZStartTimerDialog.h"
#include "ui_ZDateTimeDialog.h"

ZStartTimerDialog::ZStartTimerDialog(QWidget *parent) :
    ZCustomWidget(parent),
    ui(new Ui::ZDateTimeDialog)
{
    ui->setupUi(this);
    ui->timeEdit->hide();
    ui->dateTimeEdit->installEventFilter(this);
    ui->dateTimeEdit->setMaximumDate(QDate(2037, 12, 31));
    ui->dateTimeEdit->setMinimumDate(QDate(2010, 1, 1));
    int value;
    if (zdbus.getParameter("Main", "Recorder.Timer.Start_time", &value))
    {
        cachedValue = QDateTime::fromTime_t(value);
        ui->dateTimeEdit->setDateTime(cachedValue);
    }
}

ZStartTimerDialog::~ZStartTimerDialog()
{
    delete ui;
}

void ZStartTimerDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString ZStartTimerDialog::getValue()
{
    return cachedValue.toString(Qt::DefaultLocaleShortDate);
}

void ZStartTimerDialog::setValue(int _value)
{
    cachedValue.setTime_t(_value);
    ui->dateTimeEdit->setDateTime(cachedValue);
}


void ZStartTimerDialog::showEvent(QShowEvent *)
{
    ui->dateTimeEdit->setDateTime(cachedValue);
    ui->dateTimeEdit->setEditFocus(true);
    ui->dateTimeEdit->setCurrentSection(QDateTimeEdit::MinuteSection);
}

bool ZStartTimerDialog::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Select)
        {
            cachedValue = ui->dateTimeEdit->dateTime();
            zdbus.setParameter("Main", "Recorder.Timer.Start_time", cachedValue.toTime_t());
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    }
    return false;
}
