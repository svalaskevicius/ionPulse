/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef DBXMLAPI_H
#define DBXMLAPI_H

/**
 * \brief Provides XML DB storage integration.
 *
 * DbXml plugin provides integration with Berkeley DB XML.
 *
 * Both C++ and JS API access is provided to save, retrieve
 * and manipulate xml data.
 */

#include <QString>
#include <QMap>
#include <QDir>
#include <QSharedPointer>
#include <ionCore/plugin.h>

namespace IonDbXml {

class StorageQueryResults;


class XmlNode {
public:
    virtual ~XmlNode() {}
    typedef QMap<QString, QString> AttributesMap;
    virtual QString getName() = 0;
    virtual QString getText() = 0;
    virtual AttributesMap &getAttributes() = 0;
    virtual QVector<XmlNode*> &getChildren() = 0;
};

class StorageValue {
public:
    virtual ~StorageValue() {}

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
    virtual ~StorageQueryResults() {}

    virtual bool next() = 0;
    virtual bool previous() = 0;
    virtual bool peek() = 0;

    virtual bool hasNext() = 0;
    virtual bool hasPrevious() = 0;
    virtual void reset() = 0;

    virtual StorageValue &value() = 0;
};

class StructureStorage
{
public:
    virtual void addFile(QString path, int timestamp, XmlNode *root) = 0;
    virtual void removeFile(QString path) = 0;

    virtual void beginTransaction() = 0;
    virtual void commitTransaction() = 0;
    virtual void rollbackTransaction() = 0;

    // execute query
    // prepare query
    // execute prepared query
};



/**
 * \brief The interface of the IonDbXml plugin.
 *
 * The DbXmlPlugin interface provides an unified application scope
 * access to Berkeley DB XML based information storage and management
 * routines.
 *
 * To use it, client plugins should list IonDbXml::DbXmlPlugin::name()
 * in their dependencies. This way the client plugins will receive an
 * instance of DbXmlPlugin in their IonCore::BasicPlugin::addParent() method.
 */
class DbXmlPlugin : public IonCore::BasicPlugin
{
public:
    virtual ~DbXmlPlugin() {}

    /**
     * \brief Plugin name to be used in IonCore::BasicPlugin::getDependencies()
     *        method of the subscribed plugins.
     */
    static QString name() {return "ionDbXml";}

    /**
     * \brief Retrieve an instance of StructureStorage.
     */
    virtual StructureStorage *getStorage() = 0;
};

}

#endif // DBXMLAPI_H
