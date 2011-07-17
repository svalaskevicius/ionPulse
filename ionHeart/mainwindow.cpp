#include "mainwindow.h"
#include <QPluginLoader>
#include <QApplication>
#include <ionPulse/shared.h>
#include "ionPulse/plugin.h"
#include <QStatusBar>


namespace IonHeart {



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(QObject::tr("ionPulse"));
    setStatusBar(new QStatusBar(this));
    _loadPlugins();
}

MainWindow::~MainWindow()
{
}

QDir MainWindow::_getPluginsDir()
{
    QDir pluginsDir = QDir(qApp->applicationDirPath());
/*
#ifndef false
    #if defined(Q_OS_WIN)
        if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
            pluginsDir.cdUp();
    #elif defined(Q_OS_MAC)
        if (pluginsDir.dirName() == "MacOS") {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
    #endif
        pluginsDir.cd("plugins");
#else*/
    pluginsDir.cdUp();
    pluginsDir.cdUp();
    pluginsDir.cdUp();
    pluginsDir.cdUp();
    pluginsDir.cd("ionParticles");
    pluginsDir.cd("ionEditor");
//#endif
    DEBUG_MSG(pluginsDir.path().toStdString());
    return pluginsDir;
}


void MainWindow::_loadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        _initializePlugin(plugin);
    }

    QDir pluginsDir = _getPluginsDir();

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            DEBUG_MSG(fileName.toStdString());
            _initializePlugin(plugin);
        }
    }
}

void MainWindow::_initializePlugin(QObject *plugin)
{
    IonPlugin *plg = qobject_cast<IonPlugin *>(plugin);
    if (plg) {
        plg->initialize(this);
    }
}

}
