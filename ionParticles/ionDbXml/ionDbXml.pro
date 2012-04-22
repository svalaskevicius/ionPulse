#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../../ionPulse.pri)


TEMPLATE = lib
CONFIG += core plugin
VERSION = 0.1.0

QT       += core script
TARGET = ionDbXml
INCLUDEPATH += ../.. /opt/berkeleyDbXml/include
LIBS += -L/opt/berkeleyDbXml/lib -ldbxml-2.5

HEADERS += \
    plugin.h \
    dbxmlapi.h \
    structurestorage.h


SOURCES += \
    plugin.cpp \
    structurestorage.cpp


target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target



























