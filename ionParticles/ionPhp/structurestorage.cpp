/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "structurestorage.h"

#include <stdexcept>

namespace IonPhp {
namespace Private {

StructureStorage::StructureStorage(IonDbXml::DataStorage * dataStorage) : dataStorage(dataStorage)
{
    dataStorage->addIndex("files", "string", "edge-element-presence-none");
    dataStorage->addIndex("files", "string", "edge-element-equality-string");
    dataStorage->addIndex("files", "lineNr", "node-attribute-presence-none");
    dataStorage->addIndex("files", "class_declaration", "node-element-presence-none");
    dataStorage->addIndex("files", "METHOD", "node-element-presence-none");


    QMap<QString, QString> p;
    p["document"] = "";
    p["className"] = "";
    classQueryId = dataStorage->prepareQuery(
                    "for $x in doc($document)//class_declaration"
                    " return <ret><name>{ string($x/string) }</name><line>{ string($x/class/@lineNr) }</line></ret>"
                ,p);
    methodQueryId = dataStorage->prepareQuery(
                    "for $x in doc($document)//class_declaration[string=$className]//METHOD/string"
                    " return <ret><name>{ string($x) }</name><line>{ string($x/@lineNr) }</line></ret>"
               ,p );
}


void StructureStorage::addFile(QString path, int timestamp, ASTRoot &astRoot)
{
    dataStorage->addFile(path, timestamp, astRoot.getRootNode());
}

uint StructureStorage::getTimeStamp(QString path)
{
    return dataStorage->getTimeStamp(path);
}

IonDbXml::DataQueryResults *StructureStorage::getFileClasses(QString path)
{
//    return dataStorage->query(
//        "for $x in doc(\"dbxml:/files/"+dataStorage->pathToDocumentUri(path)+"\")//class_declaration"
//        + " return <ret><name>{ string($x/string) }</name><line>{ string($x/class/@lineNr) }</line></ret>"
//    );
    //DEBUG_MSG(classQueryId);
    QMap<QString, QString> p;
    p["document"] = "dbxml:/files/"+dataStorage->pathToDocumentUri(path);
    return dataStorage->executePrepared(classQueryId, p);
}

IonDbXml::DataQueryResults *StructureStorage::getClassMethods(QString path, QString className)
{
    //DEBUG_MSG(classQueryId);
    QMap<QString, QString> p;
    p["document"] = "dbxml:/files/"+dataStorage->pathToDocumentUri(path);
    p["className"] = className;
    return dataStorage->executePrepared(methodQueryId, p);
//    return dataStorage->query(
//        "for $x in doc(\"dbxml:/files/"+dataStorage->pathToDocumentUri(path)+"\")//class_declaration/string[text()=\""+className+"\"]/..//METHOD/string"
//        + " return <ret><name>{ string($x) }</name><line>{ string($x/@lineNr) }</line></ret>"
//    );
}


}
}
