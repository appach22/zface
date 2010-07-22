#include "ZDateTimeDialog.h"
#include "ui_ZDateTimeDialog.h"

ZDateTimeDialog::ZDateTimeDialog(QWidget *parent) :
    ZCustomWidget(parent),
    ui(new Ui::ZDateTimeDialog)
{
    ui->setupUi(this);
    ui->spinBox->installEventFilter(this);
    ui->spinBox_2->installEventFilter(this);
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
    return "00:00";
}

void ZDateTimeDialog::showEvent(QShowEvent *)
{
    ui->spinBox->setEditFocus(true);
}

bool ZDateTimeDialog::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Left ||
            static_cast<QKeyEvent *>(event)->key() == Qt::Key_Right)
        {
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    return false;
}
