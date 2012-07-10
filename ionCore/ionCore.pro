#-------------------------------------------------
#
# Project created by QtCreator 2011-07-15T20:56:55
#
#-------------------------------------------------
include (../ionPulse.pri)

QT       += core gui widgets script scripttools

TARGET = $${APPLICATION_NAME}
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        pluginloader.cpp \
        jsengine.cpp

HEADERS  += mainwindow.h \
    shared.h \
    plugin.h \
    pluginloader.h \
    jsengine.h

FORMS    +=

OTHER_FILES += \
    ionPulse.js \
    ionPulse.css \
    js/ionPulse.js \
    js/_core.js

target.path += "$${INSTALL_DIR}"
INSTALLS += target

distrib_css.path = "$${APP_TARGET_DIR}"
distrib_css.files += ionPulse.css
distrib_js.path = "$${APP_TARGET_DIR}"
distrib_js.files += js
distrib_assets.path = "$${APP_TARGET_DIR}"
distrib_assets.files += assets
js_plugins.path = "$${PLUGIN_TARGET_DIR}/"
js_plugins.files = ../lib/qtscriptgenerator/plugins/script/
INSTALLS += distrib_css distrib_js distrib_assets js_plugins












