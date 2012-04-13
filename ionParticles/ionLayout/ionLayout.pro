#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../../ionPulse.pri)


TEMPLATE = lib
CONFIG += core plugin
VERSION = 0.1.0

QT       += core widgets script
TARGET = ionLayout
INCLUDEPATH +=  ../..

HEADERS += \
    plugin.h \
    layoutapi.h \
    layoutmanager.h \
    zonenode.h \
    widgetsaggregate.h

SOURCES += \
    plugin.cpp \
    layoutmanager.cpp \
    zonenode.cpp \
    widgetsaggregate.cpp

target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target


















