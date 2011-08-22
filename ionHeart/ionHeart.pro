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
    pluginloader.cpp \
    layoutmanager.cpp

HEADERS  += mainwindow.h \
    ionPulse/shared.h \
    ionPulse/plugin.h \
    shared.h \
    plugin.h \
    pluginloader.h \
    layoutmanager.h \
    layout.h

FORMS    +=

target.path += "$${INSTALL_DIR}"
INSTALLS += target










