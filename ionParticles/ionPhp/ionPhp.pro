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
TARGET = ionPhp
INCLUDEPATH +=  ../../ionHeart/ ../ionEditor
HEADERS += \
    plugin.h \

SOURCES += \
    plugin.cpp \

target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target

