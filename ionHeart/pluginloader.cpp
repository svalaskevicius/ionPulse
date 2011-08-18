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


void PluginLoader::loadPlugins(LayoutManager &layoutManager)
{
    QString oldPwd = QDir::currentPath();
    QDir pluginsDir = _getPluginsDir();
    QDir::setCurrent(pluginsDir.absolutePath());


    QList<IPlugin *> pluginsToLoad;
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        IPlugin *plg = qobject_cast<IPlugin *>(plugin);
        if (plg) {
            pluginsToLoad.append(plg);
        }
    }

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        IPlugin *plg = qobject_cast<IPlugin *>(plugin);
        if (plg) {
            pluginsToLoad.append(plg);
        }
    }

    bool madeChanges = false;
    do {
        madeChanges = false;
        QList<IPlugin *> pluginsToLoadLater;
        foreach (IPlugin *plg, pluginsToLoad) {
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

    foreach (IPlugin *plugin, _includedPlugins.values()) {
        plugin->initialize();
        foreach (ZoneDefinition zonedef, plugin->getZoneDefinitions()) {
            layoutManager.addZone(zonedef);
        }
        foreach (IPanelWidget *panel, plugin->getPanelWidgets()) {
            layoutManager.add(panel);
        }
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


void PluginLoader::_includePlugin(IPlugin *plugin)
{
    foreach (QString dep, plugin->getDependencies()) {
        plugin->addParent(_includedPlugins[dep]);
    }
    _includedPlugins.insert(plugin->getName(), plugin);
}

}
