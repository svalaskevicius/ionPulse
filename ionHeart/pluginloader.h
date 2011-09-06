#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QMap>
#include <QDir>
#include "layoutmanager.h"

namespace IonHeart {

class BasicPlugin;

namespace Private {

class PluginLoader
{
public:
    PluginLoader();
    ~PluginLoader();
    void loadPlugins(LayoutManagerImpl &layoutManager);
private:
    QMap<QString, BasicPlugin *> _includedPlugins;
    QDir _getPluginsDir();
    bool _arePluginsIncluded(QStringList pluginNames);
    void _includePlugin(BasicPlugin *plugin);

};
}
}

#endif // PLUGINLOADER_H
