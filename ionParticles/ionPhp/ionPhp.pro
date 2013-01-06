#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../../ionPulse.pri)


TEMPLATE = lib
CONFIG += core plugin
VERSION = 0.1.0

QT       += core widgets script
TARGET = ionPhp
INCLUDEPATH += ../..
LIBS += -L../ionDbXml \
        -lionDbXml \
        -L/opt/local/lib -lz -lpthread -liconv -lm

HEADERS += \
    plugin.h \
    phpparser.h \
    phpParser/ionParserLib.h \
    structurestorage.h \
    editorsourcebrowser.h \
    phptreemodelsource.h \
    phpapi.h


SOURCES += \
    plugin.cpp \
    phpparser.cpp \
    phpParser/ionParserLib.cpp \
    structurestorage.cpp \
    editorsourcebrowser.cpp \
    phptreemodelsource.cpp


target.path += "$${PLUGIN_TARGET_DIR}"
INSTALLS += target

BISON += phpParser/php.y
FLEX +=  phpParser/php.l

bison.output = ${QMAKE_FILE_IN_PATH}/gen_${QMAKE_FILE_IN_BASE}_parser.cpp
bison.input = BISON
bison.commands = cd "${QMAKE_FILE_IN_PATH}" && bison -d -o gen_${QMAKE_FILE_IN_BASE}_parser.cpp ${QMAKE_FILE_IN_BASE}.y
bison.variable_out = SOURCES
bison.name = BISON
QMAKE_EXTRA_COMPILERS += bison

flex.output = ${QMAKE_FILE_IN_PATH}/gen_${QMAKE_FILE_IN_BASE}_scanner.cpp
flex.input = FLEX
flex.commands = cd "${QMAKE_FILE_IN_PATH}" && flex -i --header-file=gen_${QMAKE_FILE_IN_BASE}_scanner.h -ogen_${QMAKE_FILE_IN_BASE}_scanner.cpp ${QMAKE_FILE_IN_BASE}.l
flex.variable_out = SOURCES
flex.name = FLEX
QMAKE_EXTRA_COMPILERS += flex



OTHER_FILES += \
    phpParser/php.y \
    phpParser/php.l

