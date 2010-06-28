#include <qconfig.h>
#if defined(Q_WS_QWS)

#include "ZKbdHandler.h"
#include <fcntl.h>
#include <QDebug>
#include <errno.h>

const QString ZKbdHandler::inputsNames[inputsCount] = {"mainkeys", "rotary0", "rotary1", "rotary2", "hp-detect"};

ZKbdHandler::ZKbdHandler()
{
    qDebug() << "ZKbdHandler()";
    setObjectName("ANF2 Keypad Handler");
    for (int i = 0; i < inputsCount; ++i)
    {
        inputsFds[i] = open((QString("/dev/input/") + inputsNames[i]).toLocal8Bit().constData(), O_RDONLY, 0);
        if (-1 == inputsFds[i])
        {
            qDebug() << "Cannot open " << inputsNames[i] << ": " << strerror(errno);
            continue;
        }
        inputsNotifiers[i] = new QSocketNotifier(inputsFds[i], QSocketNotifier::Read, this);
        connect(inputsNotifiers[i], SIGNAL(activated(int)), this, SLOT(readKbdData(int)));
    }
}

ZKbdHandler::~ZKbdHandler()
{
    for (int i = 0; i < inputsCount; ++i)
        close(inputsFds[i]);
}

void ZKbdHandler::readKbdData(int _fd)
{
    int n = read(_fd, &event, sizeof(input_event));
    if (n < sizeof(input_event))
    {
        qDebug() << "incomplete read: " << n << " bytes";
        return;
    }

    Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    int unicode = 0xffff;
    int key_code = 0;
    int i;

    switch (event.code)
    {
        case 105 :
            key_code = Qt::Key_Left;
            break;
        case 106 :
            key_code = Qt::Key_Right;
            break;
        case 103 :
            key_code = Qt::Key_Up;
            break;
        case 108 :
            key_code = Qt::Key_Down;
            break;
        case 28 :
            key_code = Qt::Key_Select;
            break;
        case 1 :
            key_code = Qt::Key_Escape;
            break;
        case 63 :
            key_code = Qt::Key_F5;
            // Игнорируем автоповтор для кнопки записи
            if (event.value == 2)
                return;
            break;
        case 0 :
            // Обрабатываем энкодеры
            if (!event.value)
                return;
            // Определяем, от какого энкодера пришел event
            for (i = 0; inputsFds[i] != _fd && i < inputsCount; ++i){}
            if (event.value > 0)
            {
                if (inputsNames[i] == "rotary0")
                    key_code = Qt::Key_E;
                else if (inputsNames[i] == "rotary1")
                    key_code = Qt::Key_Q;
                else if (inputsNames[i] == "rotary2")
                    key_code = Qt::Key_W;
                else
                    return;
            }
            else if (event.value < 0)
            {
                if (inputsNames[i] == "rotary0")
                    key_code = Qt::Key_C;
                else if (inputsNames[i] == "rotary1")
                    key_code = Qt::Key_Z;
                else if (inputsNames[i] == "rotary2")
                    key_code = Qt::Key_X;
                else
                    return;
            }
            break;
        default :
            qDebug() << "keypressed: type=" << event.type
                                << "code=" << event.code
                                << "value=" << event.value
                                << ((event.value != 0) ? "(Down)" : "(Up)");
            return;
    }

    processKeyEvent(unicode, key_code, modifiers, event.value!=0, false);
}

#endif
