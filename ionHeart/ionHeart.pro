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
        mainwindow.cpp

HEADERS  += mainwindow.h \
    ionPulse/shared.h \
    ionPulse/plugin.h

FORMS    +=

target.path += "$${INSTALL_DIR}"
INSTALLS += target
