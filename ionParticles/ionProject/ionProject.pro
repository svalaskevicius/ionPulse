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
TARGET = ionProject
INCLUDEPATH +=  ../..

HEADERS += \
    plugin.h \
    treeitem.h \
    treewidget.h \
    treemodel.h

SOURCES += \
    plugin.cpp \
    treeitem.cpp \
    treewidget.cpp \
    treemodel.cpp

target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target














