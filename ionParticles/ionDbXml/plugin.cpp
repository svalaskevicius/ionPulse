/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include "ionCore/shared.h"

#include "datastorage.h"

namespace IonDbXml {
namespace Private {

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

Plugin::~Plugin()
{
    delete structureStorage;
}

void Plugin::preLoad()
{
    structureStorage = new DataStorageImpl();
}

}
}

Q_EXPORT_PLUGIN2 ( ionDbXml, IonDbXml::Private::Plugin )
