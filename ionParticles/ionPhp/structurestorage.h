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

class StorageQueryResults;

class StorageValue {
public:
    /// Returns the node name as per the DOM Spec or throws an
    /// XMLExpection if this value is not a node
    virtual QString getNodeName() const = 0;

    /// Returns the node value as per the DOM Spec or throws an
    /// XMLExpection if this value is not a node
    virtual QString getNodeValue() const = 0;

    /// Returns the parent node if this value is a node
    /// XMLExpection if this value is not a node
    virtual StorageValue * getParentNode() const = 0;

    /// Returns the first child node if this value is a node
    /// XMLExpection if this value is not a node
    virtual StorageValue * getFirstChild() const = 0;

    /// Returns the last child node if this value is a node
    /// XMLExpection if this value is not a node
    virtual StorageValue * getLastChild() const = 0;

    /// Returns the previous sibling node if this value is a node
    /// XMLExpection if this value is not a node
    virtual StorageValue * getPreviousSibling() const = 0;

    /// Returns the next sibling node if this value is a node
    /// XMLExpection if this value is not a node
    virtual StorageValue * getNextSibling() const = 0;

    /// Returns the attribute nodes if this value is a node
    /// XMLExpection if this value is not a node
    virtual StorageQueryResults * getAttributes() const = 0;

    /// Returns the owner element if this value is a attribute
    /// XMLExpection if this value is not a attribute
    virtual StorageValue * getOwnerElement() const = 0;

    /** @name Information Methods */

    /// Does the value have no value.
    virtual bool isNull() const = 0;

    /// Is the value of the given Type.
    virtual bool isNumber() const = 0;
    virtual bool isString() const = 0;
    virtual bool isBoolean() const = 0;
    virtual bool isBinary() const = 0;
    virtual bool isNode() const = 0;

    /** @name Accessor Methods */

    /// Return the value as a Number.
    virtual double asNumber() const = 0;
    /// Return the value as a String.
    virtual QString asString() const = 0;
    /// Return the value as a Boolean.
    virtual bool asBoolean() const = 0;
};

class StorageQueryResults {
public:
    virtual bool next() = 0;
    virtual bool previous() = 0;
    virtual bool peek() = 0;

    virtual bool hasNext() = 0;
    virtual bool hasPrevious() = 0;
    virtual void reset() = 0;

    virtual StorageValue &value() = 0;
};


class StorageQueryResultsImpl;

class StorageValueImpl : public StorageValue {
public:
    DbXml::XmlValue value;

    StorageValueImpl() {}
    StorageValueImpl(const DbXml::XmlValue &value) : value(value) {}

    /// Returns the node name as per the DOM Spec or throws an
    /// XMLExpection if this value is not a node
    QString getNodeName() const {return QString::fromStdString(value.getNodeName());}

    /// Returns the node value as per the DOM Spec or throws an
    /// XMLExpection if this value is not a node
    QString getNodeValue() const {return QString::fromStdString(value.getNodeValue());}

    /// Returns the parent node if this value is a node
    /// XMLExpection if this value is not a node
    StorageValue * getParentNode() const {return new StorageValueImpl(value.getParentNode());}

    /// Returns the first child node if this value is a node
    /// XMLExpection if this value is not a node
    StorageValue * getFirstChild() const {return new StorageValueImpl(value.getFirstChild());}

    /// Returns the last child node if this value is a node
    /// XMLExpection if this value is not a node
    StorageValue * getLastChild() const {return new StorageValueImpl(value.getLastChild());}

    /// Returns the previous sibling node if this value is a node
    /// XMLExpection if this value is not a node
    StorageValue * getPreviousSibling() const {return new StorageValueImpl(value.getPreviousSibling());}

    /// Returns the next sibling node if this value is a node
    /// XMLExpection if this value is not a node
    StorageValue * getNextSibling() const {return new StorageValueImpl(value.getNextSibling());}

    /// Returns the attribute nodes if this value is a node
    /// XMLExpection if this value is not a node
    StorageQueryResults * getAttributes() const;

    /// Returns the owner element if this value is a attribute
    /// XMLExpection if this value is not a attribute
    StorageValue * getOwnerElement() const {return new StorageValueImpl(value.getOwnerElement());}

    /** @name Information Methods */

    /// Does the value have no value.
    bool isNull() const {return value.isNull();}

    /// Is the value of the given Type.
    bool isNumber() const {return value.isNumber();}
    bool isString() const {return value.isString();}
    bool isBoolean() const {return value.isBoolean();}
    bool isBinary() const {return value.isBinary();}
    bool isNode() const {return value.isNode();}

    /** @name Accessor Methods */

    /// Return the value as a Number.
    double asNumber() const {return value.asNumber();}
    /// Return the value as a String.
    QString asString() const {return QString::fromStdString(value.asString());}
    /// Return the value as a Boolean.
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
