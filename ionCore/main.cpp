/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QtGui/QApplication>
#include "mainwindow.h"
#include "pluginloader.h"
#include <QMessageBox>
#include <stdexcept>

/**
 * \mainpage Overview
 *
 * This documentation contains API definitions for both the main IonPulse
 * application under IonCore namespace and the included set of plugins
 * each under their own namespace.
 *
 * Please see *Namespaces* section for more detailed information and convenient
 * navigation.
 */


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IonCore::Private::MainWindow w;
    IonCore::Private::PluginLoader plugins;
    try {
        plugins.loadPlugins(w);
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
