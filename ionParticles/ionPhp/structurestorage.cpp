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

StructureStorage::StructureStorage()
{
}


void StructureStorage::addFile(QString path, int timestamp, ASTRoot &astRoot)
{
//    XmlUpdateContext uc = mgr.createUpdateContext();
//    XmlDocument doc = mgr.createDocument();
//    doc.setName(path.toStdString());
//    DEBUG_MSG("numdoc" << getXmlContainer("files")->getNumDocuments());
//    XmlEventWriter &eventWriter = getXmlContainer("files")
//                                    ->putDocumentAsEventWriter(
//                                        doc,
//                                        uc
//                                    );
//    eventWriter.writeStartDocument(NULL, NULL, NULL);
//    _writeEventsForNode(eventWriter, astRoot.getRootNode());
//    eventWriter.writeEndDocument();
//    eventWriter.close();

//    getXmlContainer("filetimes")->putDocument(
//        path.toStdString(),
//        QString("<filetime>%1</filetime>").arg(timestamp).toStdString(),
//        uc
//    );

}



}
}
