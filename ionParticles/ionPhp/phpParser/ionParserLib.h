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
    ASTNode(QString name);
    ~ASTNode();
    xmlNodePtr getXmlNode();
    pASTNode setPosition(int lineNr, int columnNr);
    int getLine() const;
    int getColumn() const;
    pASTNode addChild(pASTNode child);
    pASTNode setData(QString name, QString data);
    pASTNode setText(QString data);
    QString getName();
    QString getData(QString name);
    QString getText();
    static pASTNode create(QString name);
    static void destroy(pASTNode node);
};

class ASTRoot {
protected:
    pASTNode rootNode;
    xmlDocPtr xmlDoc;
    xmlXPathContextPtr xpathCtx;
public:
    ASTRoot(pASTNode rootNode);
    ~ASTRoot();
    QString dumpXml();
    QList<pASTNode> xpath(QString xpathExpr, pASTNode parent = NULL);
};

}

#endif // IONPARSERLIB_H
