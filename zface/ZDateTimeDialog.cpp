#include "ZDateTimeDialog.h"
#include "ui_ZDateTimeDialog.h"

#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <fcntl.h>


#include <QDebug>
#include <QMessageBox>


ZDateTimeDialog::ZDateTimeDialog(QWidget *parent, const QString & _header) :
    ZCustomWidget(parent),
    ui(new Ui::ZDateTimeDialog)
{
    ui->setupUi(this);
    ui->timeEdit->hide();
    ui->dateTimeHeaderLabel->setText(_header);
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
    return ui->dateTimeEdit->date().toString(Qt::SystemLocaleShortDate);
}

void ZDateTimeDialog::setValue(int)
{
    ;
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
            unlink("/etc/timestamp");

            timeval tv;
            tv.tv_sec = ui->dateTimeEdit->dateTime().toTime_t();
            tv.tv_usec = 0;
            settimeofday(&tv, NULL);

            int fd = open("/dev/rtc0", O_WRONLY);
            if (fd < 0)
            {
                QMessageBox::critical(this, "", trUtf8("Не удалось установить время!"), QMessageBox::Ok, QMessageBox::Ok);
                return true;
            }
            else
            {
                time_t secs;
                secs = ui->dateTimeEdit->dateTime().toTime_t();

                struct tm * tm = gmtime(&secs);
                ioctl(fd, RTC_SET_TIME, tm);

                ::close(fd);
            }

            QCoreApplication::sendEvent(this, event);
            return true;
        }
    }
    return false;
}
