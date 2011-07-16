#include "mainwindow.h"
#include <QPluginLoader>
#include <QApplication>
#include <ionshared.h>
#include "ionPlugin.h"



IonHeartWindow::IonHeartWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _loadPlugins();
}

IonHeartWindow::~IonHeartWindow()
{
}

QDir IonHeartWindow::_getPluginsDir()
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


void IonHeartWindow::_loadPlugins()
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

void IonHeartWindow::_initializePlugin(QObject *plugin)
{
    IonPlugin *plg = qobject_cast<IonPlugin *>(plugin);
    if (plg) {
        DEBUG_MSG("go");
        plg->initialize(this);
    }
}
