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
    QString name;
    QVector<pASTNode> children;
    QMap<QString, QString> strdata;
    int lineNr, columnNr;
    xmlNodePtr xmlNode;
public:
    ASTNode(QString name) : name(name), lineNr(-1), columnNr(-1) {
        xmlNode = xmlNewNode(NULL, BAD_CAST name.toAscii().constData());
    }
    ~ASTNode() {foreach (pASTNode n, children) delete n; children.clear();}
    xmlNodePtr getXmlNode() {return xmlNode;}
    QString getName() {return name;}
    pASTNode setPosition(int lineNr, int columnNr) {
        this->lineNr = lineNr; this->columnNr = columnNr;
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
        strdata[name]=data;
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
    QString getStrData(QString name) {return strdata[name];}

    static pASTNode create(QString name) {return new ASTNode(name);}

    QString toMlString(int indentLevel = 0, bool printPosition = false);
    QString toString() {return toMlString(-1, false);}
    QList<pASTNode> findChildren(QString name);
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

    pASTNode operator -> () {return rootNode;}
};

}

#endif // IONPARSERLIB_H
