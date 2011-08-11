#ifndef IONPARSERLIB_H
#define IONPARSERLIB_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVector>
#include <QSharedPointer>

class ASTNode;
typedef ASTNode *pASTNode;
typedef QSharedPointer<ASTNode> ASTRoot;

class ASTNode {
protected:
    QString name;
    QList<pASTNode> children;
    QMap<QString, QString> strdata;
public:
    ASTNode(QString name) : name(name) {}
    ~ASTNode() {foreach (pASTNode n, children) delete n; children.clear();}
    QString getName() {return name;}
    pASTNode addChild(pASTNode child) {Q_ASSERT(child); children.append(child);return this;}
    pASTNode setData(QString name, QString data) {strdata[name]=data; return this;}
    QString getStrData(QString name) {return strdata[name];}
    static pASTNode create(QString name) {return new ASTNode(name);}
    QString toMlString(int indentLevel = 0);
    QString toString() {return toMlString(-1);}
};

#endif // IONPARSERLIB_H
