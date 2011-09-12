#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:26:04
#
#-------------------------------------------------
include (../ionPulse.pri)

QT       += testlib

#QT       -= gui

TARGET = tst_iontest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH +=  ..
LIBS += -L../ionParticles/ionPhp \
            -lionPhp \
            -L../ionParticles/ionEditor \
            -lionEditor


SOURCES += tst_iontest.cpp \
    phpparsertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target

HEADERS += \
    phpparsertest.h


