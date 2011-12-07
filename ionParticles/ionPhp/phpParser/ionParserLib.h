/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONPARSERLIB_H
#define IONPARSERLIB_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVector>
#include <QSharedPointer>

#include <stdexcept>

#include <ionHeart/shared.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

namespace IonPhp {

class ASTNode;
typedef ASTNode *pASTNode;

class ASTNode {
protected:
    QVector<pASTNode> children;
    xmlNodePtr xmlNode;
public:
    ASTNode(QString name) {
        xmlNode = xmlNewNode(NULL, BAD_CAST name.toAscii().constData());
    }
    ~ASTNode() {foreach (pASTNode n, children) delete n; children.clear();}
    xmlNodePtr getXmlNode() {return xmlNode;}
    pASTNode setPosition(int lineNr, int columnNr) {
        xmlNewProp(xmlNode, BAD_CAST "lineNr", BAD_CAST QString("%1").arg(lineNr).toAscii().constData());
        xmlNewProp(xmlNode, BAD_CAST "columnNr", BAD_CAST QString("%1").arg(columnNr).toAscii().constData());
        return this;
    }
    pASTNode addChild(pASTNode child) {
        if(!child) {throw std::invalid_argument( "child must be set" );};
        children.append(child);
        xmlAddChild(getXmlNode(), child->getXmlNode());
        return this;
    }
    pASTNode setData(QString name, QString data) {
        if ("text" != name) {
            xmlNewProp(xmlNode, BAD_CAST name.toAscii().constData(), BAD_CAST data.toAscii().constData());
        } else {
            xmlAddChild(
                xmlNode,
                xmlNewText(BAD_CAST  data.toAscii().constData())
            );
        }
        return this;
    }
    QString getName() { return (const char *)xmlNode->name; }

    QString getStrData(QString name) {
        if ("text" != name) {
            return (const char *) xmlGetProp(xmlNode, BAD_CAST name.toAscii().constData());
        } else {
            return (const char *) xmlNodeGetContent(xmlNode);
        }
    }

    static pASTNode create(QString name) {return new ASTNode(name);}

    pASTNode getChild(int nr) {if (children.count() <= nr) throw std::out_of_range(QString("child nr (%1) is more than children cnt (%2)").arg(nr).arg(children.count()).toStdString()); return children[nr];}
};

class ASTRoot {
protected:
    pASTNode rootNode;
    xmlDocPtr xmlDoc;
public:
    ASTRoot(pASTNode rootNode) : rootNode(rootNode), xmlDoc(xmlNewDoc(BAD_CAST "1.0"))
    {
        xmlDocSetRootElement(xmlDoc, rootNode->getXmlNode());
    }
    ~ASTRoot()
    {
        xmlFreeDoc(xmlDoc);
        delete rootNode;
    }
    QString dumpXml() {
        xmlChar *xmlbuff;
        int buffersize;

        xmlDocDumpFormatMemory(xmlDoc, &xmlbuff, &buffersize, 1);
        QString ret = (char *) xmlbuff;

        xmlFree(xmlbuff);

        return ret;
    }

};

}

#endif // IONPARSERLIB_H
