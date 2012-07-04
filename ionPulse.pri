defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

isEmpty(IDE_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    IDE_BUILD_TREE = $$cleanPath($$OUT_PWD)
    IDE_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}
INSTALL_DIR = $$IDE_BUILD_TREE/bin
APPLICATION_NAME = ionPulse
PLUGIN_TARGET_DIR = "$${INSTALL_DIR}/plugins/"
mac:PLUGIN_TARGET_DIR = "$${INSTALL_DIR}/$${APPLICATION_NAME}.app/plugins/"
APP_TARGET_DIR = "$${INSTALL_DIR}/"
mac:APP_TARGET_DIR = "$${INSTALL_DIR}/$${APPLICATION_NAME}.app/"

#CONFIG += exceptions
#QMAKE_CXXFLAGS += -std=c++0x
# -fexceptions -shared-libgcc
#QMAKE_LFLAGS +=  -shared-libgcc -Wl,-map -Wl,a
#QMAKE_CXXFLAGS += -std=c++0x -fexceptions -shared-libgcc
#QMAKE_CXXFLAGS += -fexceptions -shared-libgcc
#QMAKE_LFLAGS += -fexceptions -shared-libgcc

#QMAKE_CFLAGS_DEBUG += -fprofile-arcs -ftest-coverage -Wall
#QMAKE_CXXFLAGS_DEBUG += -fprofile-arcs -ftest-coverage -Wall
#QMAKE_LFLAGS_DEBUG += -fprofile-arcs -ftest-coverage -lgcov
