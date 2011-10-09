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

namespace IonPhp {

class ASTNode;
typedef ASTNode *pASTNode;
typedef QSharedPointer<ASTNode> ASTRoot;

class ASTNode {
protected:
    QString name;
    QList<pASTNode> children;
    QMap<QString, QString> strdata;
    int lineNr, columnNr;
public:
    ASTNode(QString name) : name(name), lineNr(-1), columnNr(-1) {}
    ~ASTNode() {foreach (pASTNode n, children) delete n; children.clear();}
    QString getName() {return name;}
    pASTNode setPosition(int lineNr, int columnNr) {this->lineNr = lineNr; this->columnNr = columnNr; return this;}
    pASTNode addChild(pASTNode child) {Q_ASSERT(child); children.append(child);return this;}
    pASTNode setData(QString name, QString data) {strdata[name]=data; return this;}
    QString getStrData(QString name) {return strdata[name];}
    static pASTNode create(QString name) {return new ASTNode(name);}
    QString toMlString(int indentLevel = 0, bool printPosition = false);
    QString toString() {return toMlString(-1, false);}
};

}

#endif // IONPARSERLIB_H
