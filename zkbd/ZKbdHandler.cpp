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
    qDebug() << "keypressed: type=" << event.type
                        << "code=" << event.code
                        << "value=" << event.value
                        << ((event.value != 0) ? "(Down)" : "(Up)");
}
