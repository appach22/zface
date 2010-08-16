# -------------------------------------------------
# Project created by QtCreator 2010-03-18T23:31:55
# -------------------------------------------------
TARGET = zface
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    ZSettingsModel.cpp \
    ZSettingsNode.cpp \
    ZSettingWidget.cpp \
    ZAllSettings.cpp \
    ZDbus.cpp \
    zfacedecoration.cpp \
    ZParamDelegate.cpp \
    ZDateTimeDialog.cpp \
    ZCustomWidget.cpp \
    ZStartTimerDialog.cpp \
    ZTimerDurationDialog.cpp \
    ZPinCode.cpp \
    ZKeyboardButton.cpp
HEADERS += mainwindow.h \
    ZSettingsModel.h \
    ZSettingsNode.h \
    ZSettingWidget.h \
    ZAllSettings.h \
    ZParameter.h \
    ZDbus.h \
    zfacedecoration.h \
    ZParamDelegate.h \
    ZDateTimeDialog.h \
    ZCustomWidget.h \
    ZStartTimerDialog.h \
    ZTimerDurationDialog.h \
    ZPinCode.h \
    ZKeyboardButton.h
QT += dbus network
DEFINES += QT_KEYPAD_NAVIGATION
FORMS += mainwindow.ui \
    ZDateTimeDialog.ui \
    ZPinCode.ui
RESOURCES += images.qrc
LIBS += ../zkbd/libzkbd.a
target.path += $$[DESTDIR]/usr/bin

# target.files += $TARGET
configs.path = /etc/zface
configs.files = settings-ru.xml \
    res/style.qss
INSTALLS += target \
    configs
