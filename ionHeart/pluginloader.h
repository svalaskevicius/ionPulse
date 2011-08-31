#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QMap>
#include <QDir>
#include "layoutmanager.h"

namespace IonHeart {

class IPlugin;

namespace Private {

class PluginLoader
{
public:
    PluginLoader();
    ~PluginLoader();
    void loadPlugins(LayoutManager &layoutManager);
private:
    QMap<QString, IPlugin *> _includedPlugins;
    QDir _getPluginsDir();
    bool _arePluginsIncluded(QStringList pluginNames);
    void _includePlugin(IPlugin *plugin);

};
}
}

#endif // PLUGINLOADER_H
