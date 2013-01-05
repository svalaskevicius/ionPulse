/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef DBXMLAPI_H
#define DBXMLAPI_H

#include <QString>
#include <QMap>
#include <QDir>
#include <QSharedPointer>
#include <ionCore/plugin.h>


/**
 * \brief Provides XML DB storage integration.
 *
 * DbXml plugin provides integration with Berkeley DB XML.
 *
 * Both C++ and JS API access is provided to save, retrieve
 * and manipulate xml data.
 */
namespace IonDbXml {

class DataQueryResults;


/**
 * \brief The interface of a node used to insert new data.
 *
 * \see DataStorage::addFile()
 */
class XmlNode : public QObject {
    Q_OBJECT
public:
    virtual ~XmlNode() {}

    /**
     * \brief Node attributes type definition
     */
    typedef QMap<QString, QString> AttributesMap;

    /**
     * \brief Node name
     */
    Q_INVOKABLE virtual QString getName() = 0;

    /**
     * \brief Node content text
     */
    Q_INVOKABLE virtual QString getText() = 0;

    /**
     * \brief Node attributes
     */
    Q_INVOKABLE virtual AttributesMap &getAttributes() = 0;

    /**
     * \brief Node children
     */
    Q_INVOKABLE virtual QVector<XmlNode*> &getChildren() = 0;
};

/**
 * \brief Xml node retrieved as a result of a storage query
 */
class DataValue : public QObject
{
    Q_OBJECT
public:
    virtual ~DataValue() {}

    /**
     * \brief Returns the node name as per the DOM Spec
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual QString getNodeName() const = 0;

    /**
     * \brief Returns the node value as per the DOM Spec
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual QString getNodeValue() const = 0;

    /**
     * \brief Returns the parent node
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual IonDbXml::DataValue * getParentNode() = 0;

    /**
     * \brief Returns the first child node
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual IonDbXml::DataValue * getFirstChild() = 0;

    /**
     * \brief Returns the last child node
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual IonDbXml::DataValue * getLastChild() = 0;

    /**
     * \brief Returns the previous sibling node
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual IonDbXml::DataValue * getPreviousSibling() = 0;

    /**
     * \brief Returns the next sibling node
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual IonDbXml::DataValue * getNextSibling() = 0;

    /**
     * \brief Returns the attribute node collection
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual IonDbXml::DataQueryResults * getAttributes() = 0;

    /**
     * \brief Returns the owner element
     *
     * If this value is not a node an XMLExpection is thrown
     */
    Q_INVOKABLE virtual IonDbXml::DataValue * getOwnerElement() = 0;

    /**
     * \brief Check if the contained value is null
     */
    Q_INVOKABLE virtual bool isNull() const = 0;

    /**
     * \brief Check if the contained value is a number
     */
    Q_INVOKABLE virtual bool isNumber() const = 0;

    /**
     * \brief Check if the contained value is string
     */
    Q_INVOKABLE virtual bool isString() const = 0;

    /**
     * \brief Check if the contained value is boolean
     */
    Q_INVOKABLE virtual bool isBoolean() const = 0;

    /**
     * \brief Check if the contained value is binary data
     */
    Q_INVOKABLE virtual bool isBinary() const = 0;

    /**
     * \brief Check if the contained value is a node
     */
    Q_INVOKABLE virtual bool isNode() const = 0;

    /**
     * \brief Retrieve the contained value as number
     */
    Q_INVOKABLE virtual double toNumber() const = 0;

    /**
     * \brief Retrieve the contained value as string
     */
    Q_INVOKABLE virtual QString toString() const = 0;

    /**
     * \brief Retrieve the contained value as boolean
     */
    Q_INVOKABLE virtual bool toBoolean() const = 0;
};

/**
 * \brief Xml node results cursor retrieved as a result of a storage query
 */
class DataQueryResults : public QObject
{
    Q_OBJECT
public:
    virtual ~DataQueryResults() {}

    /**
     * \brief Move to the next result and return true
     *
     * If there is no next result, returns false and sets the value to null
     */
    Q_INVOKABLE virtual bool next() = 0;

    /**
     * \brief Move to the previous result and return true
     *
     * If there is no previous result, returns false and sets the value to null.
     * The method can only be called on eagerly evaluated result sets.
     */
    Q_INVOKABLE virtual bool previous() = 0;

    /**
     * \brief Check if there is a current value assigned
     */
    Q_INVOKABLE virtual bool peek() = 0;

    /**
     * \brief Check if there is a next value available
     */
    Q_INVOKABLE virtual bool hasNext() = 0;

    /**
     * \brief Check if there is a previous value available
     *
     * The method can only be called on eagerly evaluated result sets.
     */
    Q_INVOKABLE virtual bool hasPrevious() = 0;

    /**
     * \brief Resets the cursor to the initial position
     *
     * The method can only be called on eagerly evaluated result sets.
     */
    Q_INVOKABLE virtual void reset() = 0;

    /**
     * \brief Retrieve current value data
     */
    Q_INVOKABLE virtual IonDbXml::DataValue *value() = 0;
};

/**
 * \brief Data storage manager class
 *
 * Provides access to database modification and access facilities.
 */
class DataStorage : public QObject
{
    Q_OBJECT
public:
    virtual ~DataStorage() {}

    /**
     * \brief Add new XML file.
     *
     * If there is a document with the same name already added, throws an exception
     */
    Q_INVOKABLE virtual void addFile(QString path, int timestamp, XmlNode *root) = 0;

    /**
     * \brief Retrieve document, added to the storage, timestamp
     */
    Q_INVOKABLE virtual uint getTimeStamp(QString path) = 0;

    /**
     * \brief Removed a document from the data storage
     */
    Q_INVOKABLE virtual void removeFile(QString path) = 0;

    /**
     * \brief Evaluate the given XQuery and retrieve its results
     */
    Q_INVOKABLE virtual IonDbXml::DataQueryResults *query(QString xquery) = 0;

    /**
     * \brief Prepare a query for faster invokation at later times
     */
    Q_INVOKABLE virtual int prepareQuery(QString xquery, QMap<QString,QString> params) = 0;

    /**
     * \brief Execute a prepared query
     */
    Q_INVOKABLE virtual IonDbXml::DataQueryResults *executePrepared(int queryId, const QMap<QString,QString> &params) = 0;

    /**
     * \brief Retrieve last database error description
     */
    Q_INVOKABLE virtual QString getLastError() = 0;

    /**
     * \brief Convert a given path to usable XQuery document name
     *
     * As BDB XML XQuery doc() function does not allow to use / for a document
     * path, use this method to transform it to urlencoded string to identify a
     * document within a collection.
     */
    Q_INVOKABLE virtual QString pathToDocumentUri(QString path) = 0;

    /**
     * \brief Create database index
     */
    Q_INVOKABLE virtual void addIndex(QString container, QString node, QString index) = 0;
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
