/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "datastorage.h"

#include <stdexcept>
#include <QUrl>
#include <ionCore/shared.h>

namespace IonDbXml {
namespace Private {

using namespace DbXml;

DataQueryResults * DataValueImpl::getAttributes()
{
    DataQueryResults *ret = new DataQueryResultsImpl(value.getAttributes());
    ret->setParent(this);
    return ret;
}


DataStorageImpl::DataStorageImpl() {
    u_int32_t flags = (DB_CREATE|DB_INIT_LOCK|DB_INIT_LOG|DB_INIT_MPOOL|DB_PRIVATE);
    DB_ENV *bdb_env;

    int ret = db_env_create(&bdb_env, flags);
    if (ret) {
        throw std::runtime_error(QString("Environment open failed: %1").arg(db_strerror(ret)).toStdString());
    }

    bdb_env->set_cachesize(bdb_env, 0, 1024*1024*1024, 1);
    bdb_env->set_errpfx(bdb_env, "BDB XML");
    bdb_env->set_errfile(bdb_env, stderr);
    bdb_env->mutex_set_increment(bdb_env, 5000);
    bdb_env->set_mp_max_openfd(bdb_env, 50);

    ret = bdb_env->log_set_config(bdb_env, DB_LOG_IN_MEMORY, 1);
    if (ret != 0) {
        throw std::runtime_error(QString("Attempt failed to set logging in memory: %1").arg(db_strerror(ret)).toStdString());
    }

    bdb_env->open(bdb_env, NULL, flags, 0);

    xmlManager = new DbXml::XmlManager(bdb_env, DbXml::DBXML_ADOPT_DBENV);

    DbXml::XmlUpdateContext up = xmlManager->createUpdateContext();
    getXmlContainer("files")->setAutoIndexing(false, up);
    getXmlContainer("filetimes")->setAutoIndexing(false, up);


    default_query_context = xmlManager->createQueryContext();
    default_query_context.setEvaluationType(DbXml::XmlQueryContext::Lazy);
    default_query_context.setDefaultCollection("files");

    QMap<QString, QString> p;
    p["document"] = "";
    timeStampQueryId = this->prepareQuery(
                    "doc($document)/filetime/text()"
                ,p);

}


void DataStorageImpl::_writeEventsForNode(XmlEventWriter &eventWriter, XmlNode *node)
{
    QByteArray name = node->getName().toAscii();
    XmlNode::AttributesMap &attributes = node->getAttributes();
    eventWriter.writeStartElement((const unsigned char*)name.constData(), NULL, NULL, attributes.count(), 0);

    for (XmlNode::AttributesMap::const_iterator it = attributes.begin(); it != attributes.end(); it++) {
        eventWriter.writeAttribute((const unsigned char*)it.key().toAscii().constData(), NULL, NULL, (const unsigned char*)it.value().toAscii().constData(), true);
    }

    foreach (XmlNode *child, node->getChildren()) {
        _writeEventsForNode(eventWriter, child);
    }

    if (node->getText().length()) {
        eventWriter.writeText(XmlEventReader::Characters, (const unsigned char*)node->getText().toAscii().constData(), node->getText().length());
    }

    eventWriter.writeEndElement((const unsigned char*)name.constData(), NULL, NULL);
}

void DataStorageImpl::addFile(QString path, int timestamp, XmlNode *root)
{
    XmlUpdateContext uc = xmlManager->createUpdateContext();
    XmlDocument doc = xmlManager->createDocument();

    QString uri = pathToDocumentUri(path);

    doc.setName(uri.toStdString());

    XmlEventWriter &eventWriter = getXmlContainer("files")
                                    ->putDocumentAsEventWriter(
                                        doc,
                                        uc
                                    );
    eventWriter.writeStartDocument(NULL, NULL, NULL);
    _writeEventsForNode(eventWriter, root);
    eventWriter.writeEndDocument();
    eventWriter.close();

    getXmlContainer("filetimes")->putDocument(
        uri.toStdString(),
        QString("<filetime>%1</filetime>").arg(timestamp).toStdString(),
        uc
    );

}

uint DataStorageImpl::getTimeStamp(QString path)
{
    QMap<QString, QString> p;
    p["document"] = "dbxml:/filetimes/"+pathToDocumentUri(path);

    DataQueryResults *pRet = executePrepared(timeStampQueryId, p);
    if (!pRet) {
        return 0;
    }
    QScopedPointer<DataQueryResults> ret(pRet);
    try {
        if (ret->next()) {
            return ret->value()->toNumber();
        }
    } catch (std::exception &e) {
        // no handling needed
    }
    return 0;
}

void DataStorageImpl::removeFile(QString path)
{
    XmlUpdateContext uc = xmlManager->createUpdateContext();

    QString uri = pathToDocumentUri(path);

    getXmlContainer("files")->deleteDocument(uri.toStdString(), uc);
    getXmlContainer("filetimes")->deleteDocument(uri.toStdString(), uc);
}

IonDbXml::DataQueryResults *DataStorageImpl::query(QString xquery)
{
    try {
        DbXml::XmlResults res = xmlManager->query(xquery.toStdString(), default_query_context);
        return (IonDbXml::DataQueryResults *) new DataQueryResultsImpl(res);
    } catch (std::exception &e) {
        lastError = e.what();
        DEBUG_MSG(lastError);
    }
    return NULL;
}




DbXml::XmlContainer *DataStorageImpl::getXmlContainer(QString name) {
    QMap<QString, DbXml::XmlContainer>::iterator it = xmlContainers.find(name);
    if (xmlContainers.end() == it) {
        QString path = getDbDir() + name + ".dbxml";
        DEBUG_MSG(path << xmlManager->existsContainer(path.toStdString()));
        if (0 != xmlManager->existsContainer(path.toStdString())) {
            DbXml::XmlContainer container = xmlManager->openContainer(path.toStdString());
            container.addAlias(name.toStdString());
            return &xmlContainers.insert(name, container).value();
        } else {
            DbXml::XmlContainer container = xmlManager->createContainer(path.toStdString());
            container.addAlias(name.toStdString());
            return &xmlContainers.insert(name, container).value();
        }
    } else {
        return &it.value();
    }
}
QString DataStorageImpl::getCollectionPath(QString name) {
    name.replace("/", "").replace("\\", "");
    return getDbDir() + name + ".dbxml";
}
QString DataStorageImpl::getDbDir() {
     QString path = QDir::homePath() + "/.ionPulse/";
     if (!QDir::home().exists(path)) {
         QDir::home().mkdir(path);
     }
     return path;
}
QString DataStorageImpl::pathToDocumentUri(QString path) {
    return QString(QUrl(path).toEncoded()).replace("/","%47");
}

int DataStorageImpl::prepareQuery(QString xquery, QMap<QString,QString> params)
{
    try {
        PreparedQuery *queryInfo = new PreparedQuery();
        queryInfo->ctx = default_query_context;
        for (QMap<QString,QString>::const_iterator it = params.begin(); it != params.end(); it++) {
            queryInfo->ctx.setVariableValue(it.key().toStdString(), it.value().toStdString());
        }
        queryInfo->query = xmlManager->prepare(xquery.toStdString(), queryInfo->ctx);
        preparedQueries.push_back(QSharedPointer<PreparedQuery>(queryInfo));
        DEBUG_MSG( QString::fromStdString(preparedQueries.back()->query.getQueryPlan()) );
        return preparedQueries.count() - 1;
    } catch (std::exception &e) {
        lastError = e.what();
        DEBUG_MSG("failed to prepare: " << lastError);
    }
    return -1;
}
IonDbXml::DataQueryResults *DataStorageImpl::executePrepared(int queryId, const QMap<QString,QString> &params)
{
    try {
        QSharedPointer<PreparedQuery> &queryInfo = preparedQueries[queryId];
        for (QMap<QString,QString>::const_iterator it = params.begin(); it != params.end(); it++) {
            queryInfo->ctx.setVariableValue(it.key().toStdString(), it.value().toStdString());
        }
        return (IonDbXml::DataQueryResults *) new DataQueryResultsImpl(queryInfo->query.execute(queryInfo->ctx));
    } catch (std::exception &e) {
        lastError = e.what();
        DEBUG_MSG("failed to execute prepared: " << lastError);
    }
    return NULL;
}

void DataStorageImpl::addIndex(QString container, QString node, QString index)
{
    XmlUpdateContext uc = xmlManager->createUpdateContext();
    getXmlContainer(container)->addIndex("", node.toStdString(), index.toStdString(), uc);
}


}
}
