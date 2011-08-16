#include <QtGui/QApplication>
#include "mainwindow.h"
#include "pluginloader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IonHeart::MainWindow w;
    IonHeart::PluginLoader().loadPlugins(&w);
    w.show();

    return a.exec();
}
