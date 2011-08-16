#include "pluginloader.h"
#include <QApplication>
#include "shared.h"
#include <QPluginLoader>
#include "plugin.h"

namespace IonHeart {

PluginLoader::PluginLoader()
{
}


QDir PluginLoader::_getPluginsDir()
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


void PluginLoader::loadPlugins(MainWindow *parent)
{
    QString oldPwd = QDir::currentPath();
    QDir pluginsDir = _getPluginsDir();
    QDir::setCurrent(pluginsDir.absolutePath());


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
                _includePlugin(plg);
                madeChanges = true;
            } else {
                pluginsToLoadLater.append(plg);
            }
        }
        pluginsToLoad = pluginsToLoadLater;
    } while (madeChanges);

    QDir::setCurrent(oldPwd);

    foreach (IonPlugin *plugin, _includedPlugins.values()) {
        plugin->initialize(parent);
    }
}

bool  PluginLoader::_arePluginsIncluded(QStringList pluginNames)
{
    foreach (QString dep, pluginNames) {
        if (!_includedPlugins.contains(dep)) {
            return false;
        }
    }
    return true;
}


void PluginLoader::_includePlugin(IonPlugin *plugin)
{
    foreach (QString dep, plugin->getDependencies()) {
        plugin->addParent(_includedPlugins[dep]);
    }
    _includedPlugins.insert(plugin->getName(), plugin);
}

}
