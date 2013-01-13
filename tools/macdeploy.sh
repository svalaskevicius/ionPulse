#!/bin/sh


if test -z "$QTDIR" -o ! -d "$QTDIR" -o ! -f "$QTDIR/bin/qmake" ; then
    echo "Please set QTDIR env variable to contain your QT installation path" >&2
    return 1
fi

if ! test -d ionPulse.app ; then
    echo "This script should be executed in a deployed app environment" >&2
    return 1
fi

rm -R ionPulse.app/assets/fonts/disabled/
find ionPulse.app -name .DS_Store -delete
"$QTDIR"/bin/macdeployqt ionPulse.app

# fix broken library links
install_name_tool -change @executable_path/../Frameworks/libionEditor.dylib @executable_path/../../plugins/libionEditor.dylib ionPulse.app/plugins/libionProject.dylib
install_name_tool -change @executable_path/../Frameworks/libionDbXml.dylib @executable_path/../../plugins/libionDbXml.dylib ionPulse.app/plugins/libionPhp.dylib


hdiutil create -format UDBZ -quiet -srcfolder ionPulse.app ionPulse.dmg

