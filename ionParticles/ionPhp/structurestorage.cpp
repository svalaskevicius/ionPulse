/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "structurestorage.h"

#include <stdexcept>

namespace IonPhp {
namespace Private {

StructureStorage::StructureStorage(IonDbXml::DataStorage * dataStorage) : dataStorage(dataStorage)
{
}


void StructureStorage::addFile(QString path, int timestamp, ASTRoot &astRoot)
{
    dataStorage->addFile(path, timestamp, astRoot.getRootNode());
}



}
}
