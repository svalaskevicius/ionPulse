#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../../ionPulse.pri)


TEMPLATE = lib
CONFIG += core plugin
VERSION = 0.1.0

QT       += core
TARGET = ionLayout
INCLUDEPATH +=  ../..

HEADERS += \
    plugin.h \
    layoutmanager.h \
    layoutapi.h

SOURCES += \
    plugin.cpp \
    layoutmanager.cpp

target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target


















