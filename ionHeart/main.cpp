/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QtGui/QApplication>
#include "mainwindow.h"
#include "pluginloader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IonHeart::Private::MainWindow w;
    IonHeart::Private::LayoutManagerImpl lm(w);
    IonHeart::Private::PluginLoader plugins;
    plugins.loadPlugins(lm);

    w.show();

    return a.exec();
}
