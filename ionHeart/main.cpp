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
