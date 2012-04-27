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
    DataStorageImpl() {
        u_int32_t flags = (/*DB_RECOVER|*/DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|DB_PRIVATE);//|DB_INIT_TXN);
        DB_ENV *bdb_env;

        int ret = db_env_create(&bdb_env, flags);
        if (ret) {
            throw std::runtime_error(QString("Environment open failed: %1").arg(db_strerror(ret)).toStdString());
        }
        bdb_env->set_cachesize(bdb_env, 0, 1024*1024*1024, 1);
        bdb_env->set_errpfx(bdb_env, "BDB XML");
        bdb_env->set_errfile(bdb_env, stderr);
        bdb_env->mutex_set_increment(bdb_env, 5000);
        //bdb_env->open(bdb_env, getDbDir().toAscii().constData(), flags, 0);
        bdb_env->open(bdb_env, NULL, flags, 0);
        uint iii;
        DEBUG_MSG(bdb_env->mutex_get_max(bdb_env, &iii));
        DEBUG_MSG(iii);

        xmlManager = new DbXml::XmlManager(bdb_env, DbXml::DBXML_ADOPT_DBENV);

        DbXml::XmlUpdateContext up = xmlManager->createUpdateContext();
        getXmlContainer("files")->setAutoIndexing(false, up);
        getXmlContainer("filetimes")->setAutoIndexing(false, up);
        getXmlContainer("files")->addIndex("", "string", "edge-element-equality-string", up);

        default_query_context = xmlManager->createQueryContext();
        default_query_context.setEvaluationType(DbXml::XmlQueryContext::Lazy);
        default_query_context.setDefaultCollection("files");

        QMap<QString, QString> p;
        p["document"] = "";
        timeStampQueryId = this->prepareQuery(
                        "doc($document)/filetime/text()"
                    ,p);

    }

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

protected:
    QVector<DbXml::XmlQueryExpression> preparedQueries;
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
