#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../ionPulse.pri)

QT       += core gui

TARGET = ionHeart
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        pluginloader.cpp

HEADERS  += mainwindow.h \
    shared.h \
    plugin.h \
    pluginloader.h

FORMS    +=

target.path += "$${INSTALL_DIR}"
INSTALLS += target










