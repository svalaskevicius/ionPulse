/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "structurestorage.h"

#include <stdexcept>

namespace IonDbXml {
namespace Private {

using namespace DbXml;

StorageQueryResults * StorageValueImpl::getAttributes() const
{
    return new StorageQueryResultsImpl(value.getAttributes());
}


void StructureStorageImpl::_writeEventsForNode(XmlEventWriter &eventWriter, XmlNode *node)
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
        eventWriter.writeText(XmlEventReader::CDATA, (const unsigned char*)node->getText().toAscii().constData(), node->getText().length());
    }

    eventWriter.writeEndElement((const unsigned char*)name.constData(), NULL, NULL);
}

void StructureStorageImpl::addFile(QString path, int timestamp, XmlNode &root)
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
    _writeEventsForNode(eventWriter, &root);
    eventWriter.writeEndDocument();
    eventWriter.close();

    getXmlContainer("filetimes")->putDocument(
        path.toStdString(),
        QString("<filetime>%1</filetime>").arg(timestamp).toStdString(),
        uc
    );

}

void StructureStorageImpl::removeFile(QString path)
{
}

}
}
