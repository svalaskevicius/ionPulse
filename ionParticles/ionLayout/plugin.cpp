/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include <ionHeart/shared.h>

namespace IonLayout {

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

void Plugin::initialize()
{
}

}

Q_EXPORT_PLUGIN2 ( ionLayout, IonLayout::Plugin )
