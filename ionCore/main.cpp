/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QApplication>
#include <QMessageBox>
#include <stdexcept>
#include <QProcess>
#include <QFontDatabase>

#include "shared.h"
#include "mainwindow.h"
#include "pluginloader.h"
#include "jsengine.h"


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
 * -  Gcc
 * -  Qt 5.0 or higher;
 * -  Boost;
 * -  Bison and Flex;
 * -  Berkeley DB XML;
 * -  GoogleMock;
 *
 *
 * Before building main project, you will first need to build lib/qtscriptgenerator,
 * following the instruction in its provided README file.
 *
 * Note, if your QT installation is not on the system path, you should set QTDIR environment
 * variable e.g.
 * @code
 *   QTDIR=~/bin/Qt5.0.0/5.0.0/clang_64/
 * @endcode
 * and use specific paths to qmake, e.g.
 * @code
 *   ~/bin/Qt5.0.0/5.0.0/clang_64/bin/qmake
 * @endcode
 * also, provide your local QT includes directory when using generator, e.g.
 * @code
 *   ./generator --include-paths=~/bin/Qt5.0.0/5.0.0/clang_64/include/
 * @endcode
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
        }
    #endif

    return baseDir.absolutePath();
}



}
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    IonCore::Private::JsEngine jsEngine;
    IonCore::Private::PluginLoader plugins;
    IonCore::Private::MainWindow mainWindow;

    try {
        QString baseDir = IonCore::Private::getBaseDir();

        QDir fontsDir(baseDir+"/assets/fonts/");
        foreach (QString name, fontsDir.entryList( QStringList()<<"*.ttf" )) {
            QFontDatabase::addApplicationFont(baseDir+"/assets/fonts/"+name);
        }

        QFile styleSheetFile(baseDir+"/ionPulse.css");
        if (styleSheetFile.open(QFile::ReadOnly)) {
            app.setStyleSheet(styleSheetFile.readAll());
        }

        QStringList paths = app.libraryPaths();
        paths <<  baseDir+"/plugins/";
        paths <<  baseDir+"/plugins/script/";
        app.setLibraryPaths(paths);

        plugins.loadPlugins(mainWindow, jsEngine, baseDir+"/plugins/");

        jsEngine.getScriptEngine().globalObject().setProperty("window", jsEngine.getScriptEngine().newQObject(&mainWindow));
        jsEngine.loadFile(baseDir+"/js/ionPulse.js");


#if defined(Q_OS_MAC)
        QWidgetList widgets = mainWindow.findChildren<QWidget*>();
        foreach(QWidget* widget, widgets) {
            widget->setAttribute(Qt::WA_MacShowFocusRect, false);
        }
#endif
        mainWindow.show();

        app.setFont(mainWindow.font());

        return app.exec();
    } catch (QString &msg) {
        QMessageBox::critical(
            0,
            "Application error",
            msg+"\n\nClick Cancel to exit.",
            QMessageBox::Cancel
        );
        return 1;
    } catch (std::exception &e) {
        QMessageBox::critical(
            0,
            "Application error",
            QString(e.what())+"\n\nClick Cancel to exit.",
            QMessageBox::Cancel
        );
        return 1;
    }

}
