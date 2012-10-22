/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QMap>
#include <QDir>

#include "mainwindow.h"
#include "jsengine.h"

namespace IonCore {

class BasicPlugin;

namespace Private {

class PluginLoader
{
protected:
    class PluginsList : public QList<BasicPlugin *> {
    private:
        void _checkAndAddOrDeletePlugin(QObject *plugin);
    public:
        void addStaticPlugins();
        void addPluginsFromDir(QDir dir);
    };
public:
    PluginLoader();
    ~PluginLoader();
    void loadPlugins(MainWindow &mainWindow, JsEngine &jsEngine, QString pluginsDir);
private:
    QMap<QString, BasicPlugin *> _includedPlugins;
    bool _arePluginsIncluded(QStringList pluginNames);
    void _includePlugin(BasicPlugin *plugin);
    void _loadPluginsList(PluginsList &plugins);
};
}
}

#endif // PLUGINLOADER_H
