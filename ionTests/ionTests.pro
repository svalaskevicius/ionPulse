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
LIBS += -lmockcpp \
            -L../ionParticles/ionProject \
            -lionProject \
            -L../ionParticles/ionPhp \
            -lionPhp \
            -L../ionParticles/ionEditor \
            -lionEditor \
            -L../ionParticles/ionLayout \
            -lionLayout


SOURCES += tst_iontest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target

HEADERS += \
    phpparsertest.h \
    projecttreetest.h \
    layouttest.h \
    phpclassbrowsertest.h






