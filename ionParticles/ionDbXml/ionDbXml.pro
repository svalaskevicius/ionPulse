#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../../ionPulse.pri)


TEMPLATE = lib
CONFIG += core plugin
VERSION = 0.1.0

QT       += core script
TARGET = ionDbXml
INCLUDEPATH += ../.. /Users/Shared/dev/dbxml/patched/dbxml-2.5.16/install/include
LIBS += -L/Users/Shared/dev/dbxml/patched/dbxml-2.5.16/install/lib -ldbxml -ldb-4.8 -lxqilla -lxerces-c

HEADERS += \
    plugin.h \
    dbxmlapi.h \
    datastorage.h


SOURCES += \
    plugin.cpp \
    datastorage.cpp


target.path += "$${PLUGIN_TARGET_DIR}"
INSTALLS += target































