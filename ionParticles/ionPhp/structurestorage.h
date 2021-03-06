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
    StructureStorage(IonDbXml::DataStorage * dataStorage);

    void addFile(QString path, int timestamp, IonDbXml::XmlNode* astRoot);
    void removeFile(QString path);
    uint getTimeStamp(QString path);
    IonDbXml::DataQueryResults *getFileClasses(QString path);
    IonDbXml::DataQueryResults *getClassMethods(QString path, QString className);
protected:
private:
    IonDbXml::DataStorage * dataStorage;
    int classQueryId, methodQueryId;
};


}
}

#endif // STRUCTURESTORAGE_H
