# requires(qws)
HEADERS += ZKbdHandler.h \
    ZKbdDriverPlugin.h
SOURCES += ZKbdHandler.cpp \
    ZKbdDriverPlugin.cpp
CONFIG += static

TEMPLATE=lib
#PLUGIN_FOR=qt
#PLUGIN_TYPE=kbddrivers
CONFIG += plugin