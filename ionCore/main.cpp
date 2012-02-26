/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QtGui/QApplication>
#include <QMessageBox>
#include <stdexcept>

#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "shared.h"
#include "mainwindow.h"
#include "pluginloader.h"

/**
 * \mainpage Overview
 *
 * This documentation contains API definitions for both the main IonPulse
 * application under IonCore namespace and the included set of plugins
 * each under their own namespaces.
 *
 * Please see *Namespaces* section for the detailed information.
 *
 * To build and develop IonPulse IDE one should install:
 * -  Gcc 4.6.2 or higher, to support C++11 standard;
 * -  Qt 4.8 or higher;
 * -  Boost;
 * -  Bison and Flex;
 * -  Sqlite 3;
 * -  GoogleMock.
 *
 *
 * PLATFORM SPECIFIC instructions:
 *
 * - MAC OSX
 *
 * As Apple is not a friend of not-only-for-apple developers, one has to manually
 * build the required GCC version required.
 * Please see these links for more instructions:
 * - http://solarianprogrammer.com/2011/12/01/compiling-gcc-4-6-2-on-mac-osx-lion/
 * - http://stackoverflow.com/questions/9102030/g-4-6-adds-a-space-between-f-and-the-path-when-calling-ld-on-mac-os-x/9345746#9345746
 *
 */


namespace IonCore {
namespace Private {


QString getBaseDir()
{
    QDir baseDir = QDir(qApp->applicationDirPath());

    #if defined(Q_OS_MAC)
        if (baseDir.dirName() == "MacOS") {
            baseDir.cdUp();
            baseDir.cdUp();
            baseDir.cdUp();
        }
    #endif

    return baseDir.absolutePath();
}

}
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IonCore::Private::MainWindow w;
    IonCore::Private::PluginLoader plugins;

    try {
        QString baseDir = IonCore::Private::getBaseDir();

        QFile styleSheetFile(baseDir+"/ionPulse.css");
        if (styleSheetFile.open(QFile::ReadOnly)) {
            a.setStyleSheet(styleSheetFile.readAll());
        }

        plugins.loadPlugins(w, baseDir+"/plugins/");

        QScriptEngine scriptEngine;
        QScriptEngineDebugger debugger;
        debugger.attachTo(&scriptEngine);
        QFile jsFile(baseDir+"/ionPulse.js");
        if (jsFile.open(QFile::ReadOnly)) {
            scriptEngine.evaluate(jsFile.readAll(), "ionPulse.js");
        }


        w.show();
        return a.exec();
    } catch (QString &msg) {
        QMessageBox::critical(
            0,
            "Application error",
            msg+"\n\nClick Cancel to exit.",
            QMessageBox::Cancel
        );
        return 1;
    }

}
