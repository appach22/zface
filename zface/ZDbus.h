#ifndef ZDBUS_H
#define ZDBUS_H

#include <QObject>
#include <QTimer>
#include <QtDBus/QtDBus>

struct SoundFileInfo
{
    int openStatus;
    int channels;
    int duration;
    int sampleSize;
    int sampleRate;
    QString tags;
};


class ZDbus : public QObject
{
Q_OBJECT
public:
    explicit ZDbus(QObject *parent = 0);

    void startAliveTimer(int _interval);

    bool setParameter(const QString & _category, const QString & _name, int _value);
    bool getParameter(const QString & _category, const QString & _name, int * _value);

    //void sendRotaryEvent(const QString & _event, const QString & _action);
    void sendPlayEvent(const QString & _event, unsigned int _arg = UINT_MAX);
    bool sendOpenFileRequest(const QString & _fileName, SoundFileInfo * _info);
    int getPresetsListing(QStringList & _listing);
    int applyPreset(const QString & _presetName);
    int deletePreset(const QString & _presetName);
    int savePreset(const QString & _presetName);
    int renamePreset(const QString & _oldName, const QString & _newName);

signals:

    // FIXME: это для теста. Переделать параметры в int.
    void gainChanged(const QString _gain, const QString _value);
    void paramChanged(const QString _param, const QString _value);
    void playStateChanged(int _state);
    void playPositionChanged(int _position);
    void recDurationChanged(int _duration);
    void messageForUser(unsigned int _code, int _type);
    void screenshot();


public slots:

private slots:

    void sendAlive();

    void receiveGain(const QString, const QString _gain, const QString _value);
    void receiveParam(const QString, const QString _param, const QString _value);
    void receivePlayState(int _state);
    void receivePlayPosition(int _position);
    void receiveRecDuration(int _duration);
    void receiveUserNotify(unsigned int _code);
    void receiveUserError(unsigned int _code);
    void receiveScreenshot();

private:
    QTimer * aliveTimer;

    static QDBusConnection bus;

    int presetOperation(const QString _presetName, const QString _operation);
};

extern ZDbus * zdbus;

#endif // ZDBUS_H
