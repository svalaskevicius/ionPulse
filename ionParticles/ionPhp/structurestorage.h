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

#include <dbxml/DbXml.hpp>

namespace IonPhp {
namespace Private {

class StructureStorage
{
public:
    StructureStorage();

    void addFile(QString path, int timestamp, ASTRoot &astRoot);
    void removeFile(QString path);

    void beginTransaction() {}
    void commitTransaction() {}
    void rollbackTransaction() {}

protected:
    DbXml::XmlManager mgr;
    DbXml::XmlContainer *getXmlContainer(QString name) {
        name.replace("/", "").replace("\\", "");
        QMap<QString, DbXml::XmlContainer>::iterator it = xmlContainers.find(name);
        if (xmlContainers.end() == it) {
            QString path = getDbDir() + name + ".dbxml";
            if (mgr.existsContainer(path.toStdString())) {
                return &xmlContainers.insert(name, mgr.openContainer(path.toStdString())).value();
            } else {
                return &xmlContainers.insert(name, mgr.createContainer(path.toStdString())).value();
            }
        } else {
            return &it.value();
        }
    }
    QString getDbDir() {
         QString path = QDir::homePath() + "/.ionPulse/";
         if (!QDir::home().exists(path)) {
             QDir::home().mkdir(path);
         }
         return path;
    }
private:
    QMap<QString, DbXml::XmlContainer> xmlContainers;
    void _writeEventsForNode(DbXml::XmlEventWriter &eventWriter, pASTNode node);
};


}
}

#endif // STRUCTURESTORAGE_H
