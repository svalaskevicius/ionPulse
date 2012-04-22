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

using namespace DbXml;


StorageQueryResults * StorageValueImpl::getAttributes() const
{
    return new StorageQueryResultsImpl(value.getAttributes());
}


StructureStorage::StructureStorage()
{
}

void StructureStorage::_writeEventsForNode(XmlEventWriter &eventWriter, ASTNode *node)
{
    QByteArray name = node->getName().toAscii();
    eventWriter.writeStartElement((const unsigned char*)name.constData(), NULL, NULL, node->attributes.count(), 0);

    for (ASTNode::AttributesMap::const_iterator it = node->attributes.begin(); it != node->attributes.end(); it++) {
        eventWriter.writeAttribute((const unsigned char*)it.key().toAscii().constData(), NULL, NULL, (const unsigned char*)it.value().toAscii().constData(), true);
    }

    foreach (pASTNode child, node->children) {
        _writeEventsForNode(eventWriter, child);
    }

    if (node->text.length()) {
        eventWriter.writeText(XmlEventReader::CDATA, (const unsigned char*)node->text.toAscii().constData(), node->text.length());
    }

    eventWriter.writeEndElement((const unsigned char*)name.constData(), NULL, NULL);
}

void StructureStorage::addFile(QString path, int timestamp, ASTRoot &astRoot)
{
    XmlUpdateContext uc = mgr.createUpdateContext();
    XmlDocument doc = mgr.createDocument();
    doc.setName(path.toStdString());
    XmlEventWriter &eventWriter = getXmlContainer("files")
                                    ->putDocumentAsEventWriter(
                                        doc,
                                        uc
                                    );
    eventWriter.writeStartDocument(NULL, NULL, NULL);
    _writeEventsForNode(eventWriter, astRoot.getRootNode());
    eventWriter.writeEndDocument();
    eventWriter.close();

    getXmlContainer("filetimes")->putDocument(
        path.toStdString(),
        QString("<filetime>%1</filetime>").arg(timestamp).toStdString(),
        uc
    );

}

void StructureStorage::removeFile(QString path)
{
}

}
}
