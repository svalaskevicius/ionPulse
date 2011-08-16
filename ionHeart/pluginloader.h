#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QMap>
#include <QDir>

class IonPlugin;

namespace IonHeart {
class MainWindow;

class PluginLoader
{
public:
    PluginLoader();
    void loadPlugins(MainWindow *parent);
private:
    QMap<QString, IonPlugin *> _includedPlugins;
    QDir _getPluginsDir();
    bool _arePluginsIncluded(QStringList pluginNames);
    void _includePlugin(IonPlugin *plugin);

};

}

#endif // PLUGINLOADER_H
