/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef STRUCTURESTORAGE_H
#define STRUCTURESTORAGE_H

#include <QString>
#include <QMap>
#include <QDir>
#include <QSharedPointer>

#include "phpParser/ionParserLib.h"

namespace IonPhp {
namespace Private {


class StructureStorage
{
public:
    StructureStorage();

    void addFile(QString path, int timestamp, ASTRoot &astRoot);
protected:
private:
};


}
}

#endif // STRUCTURESTORAGE_H
