#include "ZTimerDurationDialog.h"
#include "ui_ZDateTimeDialog.h"

ZTimerDurationDialog::ZTimerDurationDialog(QWidget *parent, const QString & _header) :
    ZCustomWidget(parent),
    ui(new Ui::ZDateTimeDialog)
{
    ui->setupUi(this);
    ui->dateTimeEdit->hide();
    ui->dateTimeHeaderLabel->setText(_header);
    ui->timeEdit->installEventFilter(this);
    ui->timeEdit->setDisplayFormat("hh:mm");
    ui->timeEdit->setMaximumTime(QTime(10, 0, 0, 0));
    int value;
    if (zdbus.getParameter("Main", "Recorder.Timer.Duration", &value))
    {
        cachedValue = QTime(value / 3600, value % 3600 / 60);
        ui->timeEdit->setTime(cachedValue);
    }
}

ZTimerDurationDialog::~ZTimerDurationDialog()
{
    delete ui;
}

void ZTimerDurationDialog::changeEvent(QEvent *e)
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

QString ZTimerDurationDialog::getValue()
{
    return cachedValue.toString(ui->timeEdit->displayFormat());
}

void ZTimerDurationDialog::setValue(int _value)
{
    cachedValue = QTime(_value / 3600, _value % 3600 / 60);
    ui->timeEdit->setTime(cachedValue);
}


void ZTimerDurationDialog::showEvent(QShowEvent *)
{
    ui->timeEdit->setTime(cachedValue);
    ui->timeEdit->setEditFocus(true);
    ui->timeEdit->setCurrentSection(QDateTimeEdit::HourSection);
}

bool ZTimerDurationDialog::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Select)
        {
            cachedValue = ui->timeEdit->time();
            zdbus.setParameter("Main", "Recorder.Timer.Duration", cachedValue.hour() * 3600 + cachedValue.minute() * 60);
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    }
    return false;
}
