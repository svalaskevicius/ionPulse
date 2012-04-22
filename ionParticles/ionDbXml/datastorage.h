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
    DataValue * getParentNode() const {return new DataValueImpl(value.getParentNode());}
    DataValue * getFirstChild() const {return new DataValueImpl(value.getFirstChild());}
    DataValue * getLastChild() const {return new DataValueImpl(value.getLastChild());}
    DataValue * getPreviousSibling() const {return new DataValueImpl(value.getPreviousSibling());}
    DataValue * getNextSibling() const {return new DataValueImpl(value.getNextSibling());}
    DataQueryResults * getAttributes() const;
    DataValue * getOwnerElement() const {return new DataValueImpl(value.getOwnerElement());}

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
    void addFile(QString path, int timestamp, XmlNode *root);
    void removeFile(QString path);

    void beginTransaction() {}
    void commitTransaction() {}
    void rollbackTransaction() {}

    IonDbXml::DataQueryResults *query(QString xquery)
    {
//        XmlQueryContext context = xmlManager.createQueryContext();
//        XmlQueryExpression qe = xmlManager.prepare(q, context);
//        XmlResults results = qe.execute(context);
        DEBUG_MSG("cont name: " << QString::fromStdString(getXmlContainer("files")->getName()));
        DbXml::XmlQueryContext context = xmlManager.createQueryContext();
        DEBUG_MSG("ctx created");
        context.setDefaultCollection("files");
        DEBUG_MSG("collection set");
        try {
            DbXml::XmlResults res = xmlManager.query(xquery.toStdString(), context);
            DEBUG_MSG("got results");
            return (IonDbXml::DataQueryResults *) new DataQueryResultsImpl(res);
        } catch (std::exception &e) {
            DEBUG_MSG("error "<<e.what());
        } catch (...) {
            DEBUG_MSG("fff");
        }
return NULL;
        //todo: use default context
//        while (results.hasNext()) {
//        XmlValue xmlValue = results.next();
//        System.out.println(xmlValue.asString());
//        }
    }

protected:
    DbXml::XmlManager xmlManager;
    DbXml::XmlContainer *getXmlContainer(QString name) {
        name.replace("/", "").replace("\\", "");
        QMap<QString, DbXml::XmlContainer>::iterator it = xmlContainers.find(name);
        if (xmlContainers.end() == it) {
            QString path = getDbDir() + name + ".dbxml";
            if (xmlManager.existsContainer(path.toStdString())) {
                DbXml::XmlContainer container = xmlManager.openContainer(path.toStdString());
                container.addAlias("files");
                return &xmlContainers.insert(name, container).value();
            } else {
                DbXml::XmlContainer container = xmlManager.createContainer(path.toStdString());
                container.addAlias("files");
                return &xmlContainers.insert(name, container).value();
            }
        } else {
            return &it.value();
        }
    }
    QString getCollectionPath(QString name) {
        name.replace("/", "").replace("\\", "");
        return getDbDir() + name + ".dbxml";
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

#endif // DATASTORAGE_H
