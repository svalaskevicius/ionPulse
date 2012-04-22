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

#include "dbxmlapi.h"

#include <dbxml/DbXml.hpp>

namespace IonDbXml {
namespace Private {

class StorageValueImpl : public StorageValue {
public:
    DbXml::XmlValue value;

    StorageValueImpl() {}
    StorageValueImpl(const DbXml::XmlValue &value) : value(value) {}
    QString getNodeName() const {return QString::fromStdString(value.getNodeName());}
    QString getNodeValue() const {return QString::fromStdString(value.getNodeValue());}
    StorageValue * getParentNode() const {return new StorageValueImpl(value.getParentNode());}
    StorageValue * getFirstChild() const {return new StorageValueImpl(value.getFirstChild());}
    StorageValue * getLastChild() const {return new StorageValueImpl(value.getLastChild());}
    StorageValue * getPreviousSibling() const {return new StorageValueImpl(value.getPreviousSibling());}
    StorageValue * getNextSibling() const {return new StorageValueImpl(value.getNextSibling());}
    StorageQueryResults * getAttributes() const;
    StorageValue * getOwnerElement() const {return new StorageValueImpl(value.getOwnerElement());}

    bool isNull() const {return value.isNull();}
    bool isNumber() const {return value.isNumber();}
    bool isString() const {return value.isString();}
    bool isBoolean() const {return value.isBoolean();}
    bool isBinary() const {return value.isBinary();}
    bool isNode() const {return value.isNode();}

    double asNumber() const {return value.asNumber();}
    QString asString() const {return QString::fromStdString(value.asString());}
    bool asBoolean() const {return value.asBoolean();}
};

class StorageQueryResultsImpl : public StorageQueryResults {
public:
    StorageQueryResultsImpl(const DbXml::XmlResults &results) : results(results) {}

    bool next() {return results.next(data.value);}
    bool previous() {return results.previous(data.value);}
    bool peek() {return results.peek(data.value);}

    bool hasNext() {return results.hasNext();}
    bool hasPrevious() {return results.hasPrevious();}
    void reset() {return results.reset();}

    StorageValue &value() {return data;}
private:
    StorageValueImpl data;
    DbXml::XmlResults results;
};

class StructureStorageImpl : public StructureStorage
{
public:
    void addFile(QString path, int timestamp, XmlNode &root);
    void removeFile(QString path);
    void executeXQuery(QString query) {

    }

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
    void _writeEventsForNode(DbXml::XmlEventWriter &eventWriter, XmlNode *node);
};


}
}

#endif // STRUCTURESTORAGE_H