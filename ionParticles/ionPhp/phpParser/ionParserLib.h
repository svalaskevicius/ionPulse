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

#include <ionCore/shared.h>
#include <ionParticles/ionDbXml/dbxmlapi.h>

#define YYSTYPE pASTNode

namespace IonPhp {
namespace Private {

class ASTNode;
typedef ASTNode *pASTNode;

class ASTNode : public IonDbXml::XmlNode {
private:
    QVector<XmlNode*> children;
    AttributesMap attributes;
    QString text;
    QString name;
    int lineNr, columnNr;
public:
    ASTNode(QString name);
    ~ASTNode();
    pASTNode setPosition(int lineNr, int columnNr);
    int getLine() const;
    int getColumn() const;
    pASTNode addChild(pASTNode child);
    pASTNode setData(QString name, QString data);
    pASTNode setText(QString data);
    QString getData(QString name);

    QString getName();
    QString getText();
    AttributesMap &getAttributes() { return attributes;}
    QVector<XmlNode*> &getChildren() {return children;}

    static pASTNode create(QString name);
    static void destroy(pASTNode node);
};

class ASTRoot {
protected:
    pASTNode rootNode;
public:
    ASTRoot(pASTNode rootNode);
    ~ASTRoot();
    pASTNode getRootNode() {
        return rootNode;
    }
};

}
}

#endif // IONPARSERLIB_H
