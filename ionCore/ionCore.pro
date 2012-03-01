#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../ionPulse.pri)

QT       += core gui script scripttools

TARGET = ionCore
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        pluginloader.cpp

HEADERS  += mainwindow.h \
    shared.h \
    plugin.h \
    pluginloader.h

FORMS    +=

OTHER_FILES += \
    ionPulse.js \
    ionPulse.css

target.path += "$${INSTALL_DIR}"
INSTALLS += target

distrib_css.path = "$${INSTALL_DIR}"
distrib_css.files += ionPulse.css
distrib_js.path = "$${INSTALL_DIR}"
distrib_js.files += ionPulse.js
js_plugins.path = "$${INSTALL_DIR}/plugins/"
js_plugins.files = ../lib/qtscriptgenerator/plugins/script/
INSTALLS += distrib_css distrib_js js_plugins









