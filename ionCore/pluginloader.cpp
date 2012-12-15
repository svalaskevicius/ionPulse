/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "pluginloader.h"
#include <QApplication>
#include "shared.h"
#include <QPluginLoader>
#include "plugin.h"
#include <QtScript/QScriptExtensionPlugin>

namespace IonCore {

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


void PluginLoader::loadPlugins(MainWindow &mainWindow, JsEngine &jsEngine, QString pluginsDir)
{
    QString oldPwd = QDir::currentPath();
    QDir::setCurrent(pluginsDir);

    PluginsList pluginsToLoad;
    pluginsToLoad.addStaticPlugins();
    pluginsToLoad.addPluginsFromDir(pluginsDir);

    foreach (BasicPlugin *plugin, pluginsToLoad) {
        plugin->setMainWindow(&mainWindow);
        plugin->preLoad();
    }

    _loadPluginsList(pluginsToLoad);

    QDir::setCurrent(oldPwd);

    foreach (BasicPlugin *plugin, _includedPlugins.values()) {
        plugin->postLoad();
        plugin->registerJsApi(jsEngine.getScriptEngine());
    }
}

void PluginLoader::_loadPluginsList(PluginsList &plugins)
{
    bool madeChanges = false;
    do {
        madeChanges = false;
        PluginsList pluginsToLoadLater;
        foreach (BasicPlugin *plg, plugins) {
            if (_arePluginsIncluded(plg->getDependencies())) {
                _includePlugin(plg);
                madeChanges = true;
            } else {
                pluginsToLoadLater.append(plg);
            }
        }
        plugins = pluginsToLoadLater;
    } while (madeChanges);
    foreach (BasicPlugin *plg, plugins) {
        std::cerr << "failed to load plugin: " << plg->getName().toStdString() << std::endl;
        delete plg;
    }
}

bool PluginLoader::_arePluginsIncluded(QStringList pluginNames)
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






void PluginLoader::PluginsList::_checkAndAddOrDeletePlugin(QObject *plugin) {
    BasicPlugin *plg = qobject_cast<BasicPlugin *>(plugin);
    if (plg) {
        this->append(plg);
    } else {
        delete plugin;
    }
}

void PluginLoader::PluginsList::addStaticPlugins() {
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        _checkAndAddOrDeletePlugin(plugin);
    }
}

void PluginLoader::PluginsList::addPluginsFromDir(QDir dir) {
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        _checkAndAddOrDeletePlugin(plugin);
    }
}


}
}
