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
#include <libxml/xpath.h>

namespace IonPhp {

class ASTNode;
typedef ASTNode *pASTNode;

class ASTNode {
protected:
    QVector<pASTNode> children;
    xmlNodePtr xmlNode;
    int lineNr, columnNr;
public:
    ASTNode(QString name) : lineNr(-1), columnNr(-1) {
        xmlNode = xmlNewNode(NULL, BAD_CAST name.toAscii().constData());
        xmlNode->_private = this;
    }
    ~ASTNode() {foreach (pASTNode n, children) delete n; children.clear();}
    xmlNodePtr getXmlNode() {return xmlNode;}
    pASTNode setPosition(int lineNr, int columnNr) {
        this->lineNr = lineNr;
        this->columnNr = columnNr;
        xmlNewProp(xmlNode, BAD_CAST "lineNr", BAD_CAST QString("%1").arg(lineNr).toAscii().constData());
        xmlNewProp(xmlNode, BAD_CAST "columnNr", BAD_CAST QString("%1").arg(columnNr).toAscii().constData());
        return this;
    }
    int getLine() const {return lineNr;}
    int getColumn() const {return columnNr;}
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
};

class ASTRoot {
protected:
    pASTNode rootNode;
    xmlDocPtr xmlDoc;
    xmlXPathContextPtr xpathCtx;
public:
    ASTRoot(pASTNode rootNode) : rootNode(rootNode), xmlDoc(xmlNewDoc(BAD_CAST "1.0"))
    {
        xmlDocSetRootElement(xmlDoc, rootNode->getXmlNode());
        xpathCtx = xmlXPathNewContext(xmlDoc);
        if(xpathCtx == NULL) {
            throw std::runtime_error("Unable to create new XPath context");
        }
    }
    ~ASTRoot()
    {
        xmlXPathFreeContext(xpathCtx);
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
    QList<pASTNode> xpath(QString xpathExpr, pASTNode parent = NULL) {
        if (!parent) {
            parent = rootNode;
        }
        xpathCtx->node = parent->getXmlNode();
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST xpathExpr.toAscii().constData(), xpathCtx);
        if(xpathObj == NULL) {
            throw std::runtime_error(QString("Unable to evaluate xpath expression \"%1\"").arg(xpathExpr).toStdString());
        }
        QList<pASTNode> ret;
        for (int i = xpathObj->nodesetval->nodeNr-1 ; i>=0; i--) {
            ret.append((pASTNode)xpathObj->nodesetval->nodeTab[i]->_private);
        }
        xmlXPathFreeObject(xpathObj);
        return ret;
    }
};

}

#endif // IONPARSERLIB_H
