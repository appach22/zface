#ifndef ZKBDDRIVERPLUGIN_H
#define ZKBDDRIVERPLUGIN_H

#include <QKbdDriverPlugin>

class ZKbdDriverPlugin : public QKbdDriverPlugin 
{
Q_OBJECT
public:
    explicit ZKbdDriverPlugin(QObject *parent = 0);
    ~ZKbdDriverPlugin();

    QWSKeyboardHandler * create(const QString & driver, const QString &);
    QStringList keys() const;
};

#endif // ZKBDDRIVERPLUGIN_H
