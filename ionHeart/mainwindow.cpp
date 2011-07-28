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

    #if defined(Q_OS_MAC)
        if (pluginsDir.dirName() == "MacOS") {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
    #endif
    pluginsDir.cd("plugins");

    DEBUG_MSG(pluginsDir.path().toStdString());
    return pluginsDir;
}


void MainWindow::_loadPlugins()
{
    QDir pluginsDir = _getPluginsDir();

    QList<IonPlugin *> pluginsToLoad;
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        IonPlugin *plg = qobject_cast<IonPlugin *>(plugin);
        if (plg) {
            pluginsToLoad.append(plg);
        }
    }

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        IonPlugin *plg = qobject_cast<IonPlugin *>(plugin);
        if (plg) {
            pluginsToLoad.append(plg);
        }
    }

    bool madeChanges = false;
    do {
        madeChanges = false;
        QList<IonPlugin *> pluginsToLoadLater;
        foreach (IonPlugin *plg, pluginsToLoad) {
            if (_arePluginsIncluded(plg->getDependencies())) {
                _initializePlugin(plg);
                madeChanges = true;
            } else {
                pluginsToLoadLater.append(plg);
            }
        }
        pluginsToLoad = pluginsToLoadLater;
    } while (madeChanges);
}

bool  MainWindow::_arePluginsIncluded(QStringList pluginNames)
{
    foreach (QString dep, pluginNames) {
        if (!_includedPlugins.contains(dep)) {
            return false;
        }
    }
    return true;
}


void MainWindow::_initializePlugin(IonPlugin *plugin)
{
    plugin->initialize(this);
    foreach (QString dep, plugin->getDependencies()) {
        plugin->addParent(_includedPlugins[dep]);
    }
    _includedPlugins.insert(plugin->getName(), plugin);
}

}
