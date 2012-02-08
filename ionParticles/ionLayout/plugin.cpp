/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include <ionCore/shared.h>

#include "layoutmanager.h"


namespace IonLayout {

namespace Private {

Plugin::Plugin(QObject *parent) :
    QObject(parent), layoutManager(NULL)
{
}

void Plugin::preLoad()
{
    Q_ASSERT(mainWindow);
    layoutManager = new Private::LayoutManagerImpl(mainWindow);
}

LayoutManager *Plugin::getLayoutManager()
{
    return layoutManager;
}

}
}

Q_EXPORT_PLUGIN2 ( ionLayout, IonLayout::Private::Plugin )
