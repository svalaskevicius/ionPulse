/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <QString>
#include <QMap>
#include <QDir>
#include <QSharedPointer>

#include "dbxmlapi.h"

#include <dbxml/DbXml.hpp>
#include <ionCore/shared.h>

namespace IonDbXml {
namespace Private {

class DataValueImpl : public DataValue {
public:
    DbXml::XmlValue value;

    DataValueImpl() {}
    DataValueImpl(const DbXml::XmlValue &value) : value(value) {}
    QString getNodeName() const {return QString::fromStdString(value.getNodeName());}
    QString getNodeValue() const {return QString::fromStdString(value.getNodeValue());}
    DataValue * getParentNode() {DataValue *ret = new DataValueImpl(value.getParentNode()); ret->setParent(this); return ret;}
    DataValue * getFirstChild() {DataValue *ret = new DataValueImpl(value.getFirstChild()); ret->setParent(this); return ret;}
    DataValue * getLastChild() {DataValue *ret = new DataValueImpl(value.getLastChild()); ret->setParent(this); return ret;}
    DataValue * getPreviousSibling() {DataValue *ret = new DataValueImpl(value.getPreviousSibling()); ret->setParent(this); return ret;}
    DataValue * getNextSibling() {DataValue *ret = new DataValueImpl(value.getNextSibling()); ret->setParent(this); return ret;}
    DataQueryResults * getAttributes();
    DataValue * getOwnerElement() {DataValue *ret = new DataValueImpl(value.getOwnerElement()); ret->setParent(this); return ret;}

    bool isNull() const {return value.isNull();}
    bool isNumber() const {return value.isNumber();}
    bool isString() const {return value.isString();}
    bool isBoolean() const {return value.isBoolean();}
    bool isBinary() const {return value.isBinary();}
    bool isNode() const {return value.isNode();}

    double toNumber() const {return value.asNumber();}
    QString toString() const {return QString::fromStdString(value.asString());}
    bool toBoolean() const {return value.asBoolean();}
};

class DataQueryResultsImpl : public DataQueryResults {
public:
    DataQueryResultsImpl(const DbXml::XmlResults &results) : results(results)
    {
    }

    bool next() {return results.next(data.value);}
    bool previous() {return results.previous(data.value);}
    bool peek() {return results.peek(data.value);}

    bool hasNext() {return results.hasNext();}
    bool hasPrevious() {return results.hasPrevious();}
    void reset() {return results.reset();}

    DataValue *value() {return &data;}
private:
    DataValueImpl data;
    DbXml::XmlResults results;
};

class DataStorageImpl : public DataStorage
{
public:
    DataStorageImpl();

    ~DataStorageImpl() {
        delete xmlManager;
    }
    void addFile(QString path, int timestamp, XmlNode *root);
    uint getTimeStamp(QString path);
    void removeFile(QString path);

    void beginTransaction() {}
    void commitTransaction() {}
    void rollbackTransaction() {}

    QString getLastError() { return lastError; }

    IonDbXml::DataQueryResults *query(QString xquery);
    int prepareQuery(QString xquery, QMap<QString,QString> params);
    IonDbXml::DataQueryResults *executePrepared(int queryId, const QMap<QString,QString> &params);

    QString pathToDocumentUri(QString path);

    void addIndex(QString container, QString node, QString index);

protected:
    struct PreparedQuery {
        DbXml::XmlQueryExpression query;
        DbXml::XmlQueryContext ctx;
    };
    QVector<QSharedPointer<PreparedQuery> > preparedQueries;
    DbXml::XmlManager *xmlManager;
    DbXml::XmlQueryContext default_query_context;
    QString lastError;
    int timeStampQueryId;

    DbXml::XmlContainer *getXmlContainer(QString name);
    QString getCollectionPath(QString name);
    QString getDbDir();
private:
    QMap<QString, DbXml::XmlContainer> xmlContainers;
    void _writeEventsForNode(DbXml::XmlEventWriter &eventWriter, XmlNode *node);
};


}
}

#endif // DATASTORAGE_H
