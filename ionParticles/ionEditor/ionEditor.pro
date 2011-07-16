#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += core gui plugin
VERSION = 0.1.0

QT       += core gui
TARGET = ionEditor
INCLUDEPATH +=  ../../ionHeart/
HEADERS += \
    ioneditor.h \
    iontexteditorwidget.h \
    linenumberarea.h

SOURCES += \
    ioneditor.cpp \
    iontexteditorwidget.cpp \
    linenumberarea.cpp
