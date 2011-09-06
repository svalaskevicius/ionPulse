#include "pluginloader.h"
#include <QApplication>
#include "shared.h"
#include <QPluginLoader>
#include "plugin.h"

namespace IonHeart {

namespace Private {

PluginLoader::PluginLoader()
{
}

PluginLoader::~PluginLoader()
{
    foreach (BasicPlugin *plg, _includedPlugins) {
        delete plg;
    }
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


void PluginLoader::loadPlugins(LayoutManagerImpl &layoutManager)
{
    QString oldPwd = QDir::currentPath();
    QDir pluginsDir = _getPluginsDir();
    QDir::setCurrent(pluginsDir.absolutePath());


    QList<BasicPlugin *> pluginsToLoad;
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        BasicPlugin *plg = qobject_cast<BasicPlugin *>(plugin);
        if (plg) {
            pluginsToLoad.append(plg);
        } else {
            delete plugin;
        }
    }

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        BasicPlugin *plg = qobject_cast<BasicPlugin *>(plugin);
        if (plg) {
            pluginsToLoad.append(plg);
        } else {
            delete plugin;
        }
    }

    bool madeChanges = false;
    do {
        madeChanges = false;
        QList<BasicPlugin *> pluginsToLoadLater;
        foreach (BasicPlugin *plg, pluginsToLoad) {
            if (_arePluginsIncluded(plg->getDependencies())) {
                _includePlugin(plg);
                madeChanges = true;
            } else {
                pluginsToLoadLater.append(plg);
            }
        }
        pluginsToLoad = pluginsToLoadLater;
    } while (madeChanges);
    foreach (BasicPlugin *plg, pluginsToLoad) {
        std::cerr << "failed to load plugin: " << plg->getName().toStdString() << std::endl;
        delete plg;
    }
    QDir::setCurrent(oldPwd);

    foreach (BasicPlugin *plugin, _includedPlugins.values()) {
        plugin->setLayoutManager(&layoutManager);
        plugin->initialize();
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


void PluginLoader::_includePlugin(BasicPlugin *plugin)
{
    foreach (QString dep, plugin->getDependencies()) {
        plugin->addParent(_includedPlugins[dep]);
    }
    _includedPlugins.insert(plugin->getName(), plugin);
}
}
}
