#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../../ionPulse.pri)


TEMPLATE = lib
CONFIG += core gui plugin
VERSION = 0.1.0

QT       += core gui widgets script
TARGET = ionEditor
INCLUDEPATH +=  ../../
HEADERS += \
    linenumberarea.h \
    plugin.h \
    editorwidget.h \
    highlighter.h \
    editorwidgetfactory.h \
    filetreewidget.h \
    editorapi.h \
    searchpanel.h

SOURCES += \
    linenumberarea.cpp \
    plugin.cpp \
    editorwidget.cpp \
    highlighter.cpp \
    editorwidgetfactory.cpp \
    filetreewidget.cpp \
    searchpanel.cpp

target.path += "$${PLUGIN_TARGET_DIR}"
INSTALLS += target





