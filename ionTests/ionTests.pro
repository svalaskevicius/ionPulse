#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:26:04
#
#-------------------------------------------------
include (../ionPulse.pri)

QT       += testlib widgets sql script

#QT       -= gui

TARGET = tst_iontest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH +=  .. /opt/local/include/libxml2 /usr/local/include/ /opt/berkeleyDbXml/include

LIBS += -lgmock -lgtest \
            -L../ionParticles/ionProject \
            -lionProject \
            -L../ionParticles/ionPhp \
            -lionPhp \
            -L../ionParticles/ionEditor \
            -lionEditor \
            -L../ionParticles/ionLayout \
            -lionLayout \
            -L/opt/local/lib -lxml2 -lz -lpthread -liconv -lm


SOURCES += tst_iontest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target

HEADERS += \
    phpparsertest.h \
    projecttreetest.h \
    layouttest.h \
    phpclassbrowsertest.h






