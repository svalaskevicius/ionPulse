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
//    DataQueryResults *pRet = query("doc(\"dbxml:/filetimes/"+pathToDocumentUri(path)+"\")/filetime/text()");
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
}

IonDbXml::DataQueryResults *DataStorageImpl::query(QString xquery)
{
//        XmlQueryContext context = xmlManager.createQueryContext();
//        XmlQueryExpression qe = xmlManager.prepare(q, context);
//        XmlResults results = qe.execute(context);
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
        DbXml::XmlQueryContext ctx = default_query_context;
        for (QMap<QString,QString>::const_iterator it = params.begin(); it != params.end(); it++) {
            ctx.setVariableValue(it.key().toStdString(), it.value().toStdString());
        }
        preparedQueries.push_back(xmlManager->prepare(xquery.toStdString(), ctx));
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
        DbXml::XmlQueryContext ctx = default_query_context;
        for (QMap<QString,QString>::const_iterator it = params.begin(); it != params.end(); it++) {
            ctx.setVariableValue(it.key().toStdString(), it.value().toStdString());
        }
        return (IonDbXml::DataQueryResults *) new DataQueryResultsImpl(preparedQueries[queryId].execute(ctx));
    } catch (std::exception &e) {
        lastError = e.what();
        DEBUG_MSG("failed to execute prepared: " << lastError);
    }
    return NULL;
}

}
}
