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
    ZDbus.cpp
HEADERS += mainwindow.h \
    ZSettingsModel.h \
    ZSettingsNode.h \
    ZSettingWidget.h \
    ZAllSettings.h \
    ZParameter.h \
    ZDbus.h
FORMS += mainwindow.ui
RESOURCES += images.qrc
LIBS += ../zkbd/libzkbd.a
target.path += $$[DESTDIR]/usr/bin
#target.files += $TARGET
configs.path = /etc/zface
configs.files = settings-en.xml res/style.qss
INSTALLS += target configs
