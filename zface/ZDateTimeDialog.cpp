#include "ZDateTimeDialog.h"
#include "ui_ZDateTimeDialog.h"

#include <time.h>
#include <sys/time.h>

#include <QDebug>


ZDateTimeDialog::ZDateTimeDialog(QWidget *parent) :
    ZCustomWidget(parent),
    ui(new Ui::ZDateTimeDialog)
{
    ui->setupUi(this);
    ui->timeEdit->hide();
    ui->dateTimeEdit->installEventFilter(this);
    ui->dateTimeEdit->setMaximumDate(QDate(2037, 12, 31));
    ui->dateTimeEdit->setMinimumDate(QDate(2010, 1, 1));
}

ZDateTimeDialog::~ZDateTimeDialog()
{
    delete ui;
}

void ZDateTimeDialog::changeEvent(QEvent *e)
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

QString ZDateTimeDialog::getValue()
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    return ui->dateTimeEdit->date().toString(Qt::DefaultLocaleShortDate);
}

void ZDateTimeDialog::showEvent(QShowEvent *)
{
    ui->dateTimeEdit->setEditFocus(true);
    ui->dateTimeEdit->setCurrentSection(QDateTimeEdit::MinuteSection);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

bool ZDateTimeDialog::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Select)
        {
            timeval tv;
            tv.tv_sec = ui->dateTimeEdit->dateTime().toTime_t();
            tv.tv_usec = 0;
            settimeofday(&tv, NULL);
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    }
    return false;
}
