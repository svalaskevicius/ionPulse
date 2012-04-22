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
INCLUDEPATH += ../.. /opt/berkeleyDbXml/include
LIBS += -L/opt/local/lib -lz -lpthread -liconv -lm -L/opt/berkeleyDbXml/lib -ldbxml-2.5

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
    gen_php_parser.cpp \
    gen_php_scanner.cpp \
    phpparser.cpp \
    phpParser/ionParserLib.cpp \
    structurestorage.cpp \
    editorsourcebrowser.cpp \
    phptreemodelsource.cpp


target.path += "$${INSTALL_DIR}/plugins/"
INSTALLS += target





# middle compilers:
gen_php_parser.target = $${PWD}/phpParser/gen_php_parser.cpp
gen_php_parser.commands =  cd "$${PWD}/phpParser/" && bison -d -o gen_php_parser.cpp php.y
gen_php_parser.depends =  $${PWD}/phpParser/php.y
QMAKE_EXTRA_TARGETS += gen_php_parser

gen_php_parser_dep.target = gen_php_parser.cpp
gen_php_parser_dep.commands =  $$QMAKE_COPY $${PWD}/phpParser/gen_php_parser.cpp $${OUT_PWD}/gen_php_parser.cpp && \
                               $$QMAKE_COPY $${PWD}/phpParser/gen_php_parser.hpp $${OUT_PWD}/gen_php_parser.hpp
gen_php_parser_dep.depends =  $${PWD}/phpParser/gen_php_parser.cpp
QMAKE_EXTRA_TARGETS += gen_php_parser_dep


gen_php_scanner.target = $${PWD}/phpParser/gen_php_scanner.cpp
gen_php_scanner.commands =  cd "$${PWD}/phpParser/" && flex -i --header-file=gen_php_scanner.h -ogen_php_scanner.cpp php.l
gen_php_scanner.depends =  $${PWD}/phpParser/php.l
QMAKE_EXTRA_TARGETS += gen_php_scanner

gen_php_scanner_dep.target = gen_php_scanner.cpp
gen_php_scanner_dep.commands =  $$QMAKE_COPY $${PWD}/phpParser/gen_php_scanner.cpp $${OUT_PWD}/gen_php_scanner.cpp
gen_php_scanner_dep.depends =  $${PWD}/phpParser/gen_php_scanner.cpp
QMAKE_EXTRA_TARGETS += gen_php_scanner_dep

OTHER_FILES += \
    phpParser/php.y \
    phpParser/php.l




























