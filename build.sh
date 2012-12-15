#!/bin/bash

clean_qtscript() {
    pushd "`dirname "$0"`/lib/qtscriptgenerator" &&
    git clean -dfx || return 1
    popd
}

build_qtscript() {
    if test -z "$QTDIR" -o ! -d "$QTDIR" -o ! -f "$QTDIR/bin/qmake" ; then
        echo "Please set QTDIR env variable to contain your QT installation path" >&2
        return 1
    fi

    pushd "`dirname "$0"`/lib/qtscriptgenerator" &&
    cd generator &&
    "$QTDIR/bin/qmake" -recursive &&
    make -j4 &&
    ./generator "--include-paths=$QTDIR/include/" &&
    cd ../qtbindings/ &&
    "$QTDIR/bin/qmake" -recursive &&
    make -j4 &&
    popd || return 1
}


clean_qtscript || exit 1
build_qtscript || exit 1

