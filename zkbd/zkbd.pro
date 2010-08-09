# requires(qws)
HEADERS += ZKbdHandler.h \
    ZKbdDriverPlugin.h \
    ZKbdDbusHandler.h
SOURCES += ZKbdHandler.cpp \
    ZKbdDriverPlugin.cpp \
    ZKbdDbusHandler.cpp
CONFIG += static
TEMPLATE = lib

# PLUGIN_FOR=qt
# PLUGIN_TYPE=kbddrivers
CONFIG += plugin
