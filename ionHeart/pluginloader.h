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
