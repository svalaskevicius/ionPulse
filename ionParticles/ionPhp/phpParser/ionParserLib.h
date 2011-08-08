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
public:
    ASTNode(QString name) : name(name) {}
    pASTNode addChild(pASTNode child) {children.append(child);return this;}
    pASTNode setData(QString name, QString data) {strdata[name]=data; return this;}
    static pASTNode create(QString name) {return new ASTNode(name);}
};

//void ion_php_error(const char *error);

#endif // IONPARSERLIB_H
