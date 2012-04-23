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

class DataQueryResults;


class XmlNode {
public:
    virtual ~XmlNode() {}
    typedef QMap<QString, QString> AttributesMap;
    virtual QString getName() = 0;
    virtual QString getText() = 0;
    virtual AttributesMap &getAttributes() = 0;
    virtual QVector<XmlNode*> &getChildren() = 0;
};

class DataValue : public QObject
{
    Q_OBJECT
public:
    virtual ~DataValue() {}

    /// Returns the node name as per the DOM Spec or throws an
    /// XMLExpection if this value is not a node
    Q_INVOKABLE virtual QString getNodeName() const = 0;

    /// Returns the node value as per the DOM Spec or throws an
    /// XMLExpection if this value is not a node
    Q_INVOKABLE virtual QString getNodeValue() const = 0;

    /// Returns the parent node if this value is a node
    /// XMLExpection if this value is not a node
    Q_INVOKABLE virtual IonDbXml::DataValue * getParentNode() const = 0;

    /// Returns the first child node if this value is a node
    /// XMLExpection if this value is not a node
    Q_INVOKABLE virtual IonDbXml::DataValue * getFirstChild() const = 0;

    /// Returns the last child node if this value is a node
    /// XMLExpection if this value is not a node
    Q_INVOKABLE virtual IonDbXml::DataValue * getLastChild() const = 0;

    /// Returns the previous sibling node if this value is a node
    /// XMLExpection if this value is not a node
    Q_INVOKABLE virtual IonDbXml::DataValue * getPreviousSibling() const = 0;

    /// Returns the next sibling node if this value is a node
    /// XMLExpection if this value is not a node
    Q_INVOKABLE virtual IonDbXml::DataValue * getNextSibling() const = 0;

    /// Returns the attribute nodes if this value is a node
    /// XMLExpection if this value is not a node
    Q_INVOKABLE virtual IonDbXml::DataQueryResults * getAttributes() const = 0;

    /// Returns the owner element if this value is a attribute
    /// XMLExpection if this value is not a attribute
    Q_INVOKABLE virtual IonDbXml::DataValue * getOwnerElement() const = 0;

    /** @name Information Methods */

    /// Does the value have no value.
    Q_INVOKABLE virtual bool isNull() const = 0;

    /// Is the value of the given Type.
    Q_INVOKABLE virtual bool isNumber() const = 0;
    Q_INVOKABLE virtual bool isString() const = 0;
    Q_INVOKABLE virtual bool isBoolean() const = 0;
    Q_INVOKABLE virtual bool isBinary() const = 0;
    Q_INVOKABLE virtual bool isNode() const = 0;

    /** @name Accessor Methods */

    /// Return the value as a Number.
    Q_INVOKABLE virtual double toNumber() const = 0;
    /// Return the value as a String.
    Q_INVOKABLE virtual QString toString() const = 0;
    /// Return the value as a Boolean.
    Q_INVOKABLE virtual bool toBoolean() const = 0;
};

class DataQueryResults : public QObject
{
    Q_OBJECT
public:
    virtual ~DataQueryResults() {}

    Q_INVOKABLE virtual bool next() = 0;
    Q_INVOKABLE virtual bool previous() = 0;
    Q_INVOKABLE virtual bool peek() = 0;

    Q_INVOKABLE virtual bool hasNext() = 0;
    Q_INVOKABLE virtual bool hasPrevious() = 0;
    Q_INVOKABLE virtual void reset() = 0;

    Q_INVOKABLE virtual IonDbXml::DataValue *value() = 0;
};

class DataStorage : public QObject
{
    Q_OBJECT
public:
    virtual ~DataStorage() {}

    Q_INVOKABLE virtual void addFile(QString path, int timestamp, XmlNode *root) = 0;
    Q_INVOKABLE virtual uint getTimeStamp(QString path) = 0;
    Q_INVOKABLE virtual void removeFile(QString path) = 0;

    Q_INVOKABLE virtual void beginTransaction() = 0;
    Q_INVOKABLE virtual void commitTransaction() = 0;
    Q_INVOKABLE virtual void rollbackTransaction() = 0;

    // execute query
    Q_INVOKABLE virtual IonDbXml::DataQueryResults *query(QString xquery) = 0;
    // prepare query
    // execute prepared query
    Q_INVOKABLE virtual QString getLastError() = 0;
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
    Q_INVOKABLE virtual IonDbXml::DataStorage *getStorage() = 0;
};

}

#endif // DBXMLAPI_H
