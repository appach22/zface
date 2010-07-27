#include "ZPinCode.h"
#include "ui_ZPinCode.h"

#include <QMessageBox>

ZPinCode::ZPinCode(QWidget *parent) :
    ZCustomWidget(parent),
    ui(new Ui::ZPinCode)
{
    ui->setupUi(this);
    ui->digit1->installEventFilter(this);
    ui->digit2->installEventFilter(this);
    ui->digit3->installEventFilter(this);
    ui->digit4->installEventFilter(this);
    ui->digit5->installEventFilter(this);
    ui->digit6->installEventFilter(this);
    ui->digit7->installEventFilter(this);
    ui->digit8->installEventFilter(this);
    if (!zdbus.getParameter("Main", "Security.Protection.PIN", &cachedValue))
        cachedValue = -1;
}

ZPinCode::~ZPinCode()
{
    delete ui;
}

void ZPinCode::changeEvent(QEvent *e)
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

QString ZPinCode::getValue()
{
    return "";
}

void ZPinCode::setValue(int)
{
    ;
}

void ZPinCode::showEvent(QShowEvent *)
{
    if (-1 == cachedValue)
        QMessageBox::critical(this, "", trUtf8("Ошибка получения текущего ПИН-кода!"), QMessageBox::Ok, QMessageBox::Ok);
    ui->digit1->setValue(0);
    ui->digit2->setValue(0);
    ui->digit3->setValue(0);
    ui->digit4->setValue(0);
    ui->digit5->setValue(0);
    ui->digit6->setValue(0);
    ui->digit7->setValue(0);
    ui->digit8->setValue(0);
    ui->digit1->setEditFocus(true);
}

bool ZPinCode::eventFilter(QObject * obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Left ||
            static_cast<QKeyEvent *>(event)->key() == Qt::Key_Right)
        {
            if (obj == ui->digit4 && static_cast<QKeyEvent *>(event)->key() == Qt::Key_Right)
            {
                ui->digit5->setEditFocus(true);
                return true;
            }
            if (obj == ui->digit5 && static_cast<QKeyEvent *>(event)->key() == Qt::Key_Left)
            {
                ui->digit4->setEditFocus(true);
                return true;
            }
            QCoreApplication::sendEvent(this, event);
            QApplication::focusWidget()->setEditFocus(true);
            return true;
        }
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Select)
        {
            int oldPin = ui->digit1->value() * 1000 + ui->digit2->value() * 100 + ui->digit3->value() * 10 + ui->digit4->value();
            if (cachedValue != oldPin)
            {
                QMessageBox::warning(this, "", trUtf8("Старый ПИН-код введен неверно!"), QMessageBox::Ok, QMessageBox::Ok);
                ui->digit1->setEditFocus(true);
                return true;
            }
            int newPin = ui->digit5->value() * 1000 + ui->digit6->value() * 100 + ui->digit7->value() * 10 + ui->digit8->value();
            if (!zdbus.setParameter("Main", "Security.Protection.PIN", newPin))
            {
                QMessageBox::critical(this, "", trUtf8("Не удалось установить новый ПИН-код!"), QMessageBox::Ok, QMessageBox::Ok);
                ui->digit1->setEditFocus(true);
                return true;
            }
            cachedValue = newPin;
            QCoreApplication::sendEvent(this, event);
            return true;
        }
    }
    return false;
}
