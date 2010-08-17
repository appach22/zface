#ifndef ZKEYBOARDBUTTON_H
#define ZKEYBOARDBUTTON_H

#include <QPushButton>

class ZKeyboardButton : public QPushButton
{
Q_OBJECT
public:
    explicit ZKeyboardButton(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);

signals:

public slots:

};

#endif // ZKEYBOARDBUTTON_H
