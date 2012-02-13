/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "ionParserLib.h"
#include <iostream>

namespace IonPhp {
namespace Private {

ASTNode::ASTNode(QString name)
    : lineNr(-1), columnNr(-1)
{
    xmlNode = xmlNewNode(NULL, BAD_CAST name.toAscii().constData());
    xmlNode->_private = this;
}

ASTNode::~ASTNode()
{
    foreach (pASTNode n, children) delete n; children.clear();
}

xmlNodePtr ASTNode::getXmlNode()
{
    return xmlNode;
}

pASTNode ASTNode::setPosition(int lineNr, int columnNr)
{
    this->lineNr = lineNr;
    this->columnNr = columnNr;
    xmlNewProp(xmlNode, BAD_CAST "lineNr", BAD_CAST QString("%1").arg(lineNr).toAscii().constData());
    xmlNewProp(xmlNode, BAD_CAST "columnNr", BAD_CAST QString("%1").arg(columnNr).toAscii().constData());
    return this;
}

int ASTNode::getLine() const
{
    return lineNr;
}

int ASTNode::getColumn() const
{
    return columnNr;
}

pASTNode ASTNode::addChild(pASTNode child)
{
    if (!child) {
        throw std::invalid_argument( "child must be set" );
    }
    children.append(child);
    xmlAddChild(getXmlNode(), child->getXmlNode());
    return this;
}

pASTNode ASTNode::setData(QString name, QString data)
{
    xmlNewProp(xmlNode, BAD_CAST name.toAscii().constData(), BAD_CAST data.toAscii().constData());
    return this;
}

pASTNode ASTNode::setText(QString data)
{
    xmlAddChild(
        xmlNode,
        xmlNewText(BAD_CAST  data.toAscii().constData())
    );
    return this;
}

QString ASTNode::getName()
{
    return (const char *)xmlNode->name;
}

QString ASTNode::getData(QString name)
{
    return (const char *) xmlGetProp(xmlNode, BAD_CAST name.toAscii().constData());
}

QString ASTNode::getText()
{
    char *text = (char *) xmlNodeGetContent(xmlNode);
    QString ret = text;
    free(text);
    return ret;
}

pASTNode ASTNode::create(QString name)
{
    return new ASTNode(name);
}
void ASTNode::destroy(pASTNode node)
{
    xmlFreeNode(node->getXmlNode());
    delete node;
}



ASTRoot::ASTRoot(pASTNode rootNode)
    : rootNode(rootNode), xmlDoc(xmlNewDoc(BAD_CAST "1.0"))
{
    xmlDocSetRootElement(xmlDoc, rootNode->getXmlNode());
    xpathCtx = xmlXPathNewContext(xmlDoc);
    if(xpathCtx == NULL) {
        throw std::runtime_error("Unable to create new XPath context");
    }
}

ASTRoot::~ASTRoot()
{
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(xmlDoc);
    delete rootNode;
}

QString ASTRoot::dumpXml()
{
    xmlChar *xmlbuff;
    int buffersize;

    xmlDocDumpFormatMemory(xmlDoc, &xmlbuff, &buffersize, 1);
    QString ret = (char *) xmlbuff;

    xmlFree(xmlbuff);

    return ret;
}

QList<pASTNode> ASTRoot::xpath(QString xpathExpr, pASTNode parent) const
{
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

}
}
