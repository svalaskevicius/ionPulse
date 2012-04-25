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
LIBS += -L/opt/berkeleyDbXml/lib -ldbxml-2.5 -ldb-4.8

HEADERS += \
    plugin.h \
    dbxmlapi.h \
    datastorage.h


SOURCES += \
    plugin.cpp \
    datastorage.cpp


target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target































