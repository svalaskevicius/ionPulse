#ifndef IONPARSERLIB_H
#define IONPARSERLIB_H

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QMap>
#include <QVector>

class ASTNode;
typedef ASTNode *pASTNode;

class ASTNode {
protected:
    QString name;
    QList<pASTNode> children;
    QMap<QString, QString> strdata;
    void _print_r(int level);
public:
    ASTNode(QString name) : name(name) {}
    pASTNode addChild(pASTNode child) {Q_ASSERT(child); children.append(child);return this;}
    pASTNode setData(QString name, QString data) {strdata[name]=data; return this;}
    static pASTNode create(QString name) {return new ASTNode(name);}
    void print_r() {_print_r(0);}
};

#endif // IONPARSERLIB_H
