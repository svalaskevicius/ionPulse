#include <QtGui/QApplication>
#include "mainwindow.h"
#include "pluginloader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IonHeart::MainWindow w;
    IonHeart::LayoutManager lm(w);
    IonHeart::PluginLoader().loadPlugins(lm);
    w.show();

    return a.exec();
}
