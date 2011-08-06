#ifndef IONPARSERLIB_H
#define IONPARSERLIB_H

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QMap>


class _ASTNode;
typedef QSharedPointer<_ASTNode> ASTNode;
class _ASTNode {
protected:
    QString name;
    QList<ASTNode> children;
    QMap<QString, QString> strdata;
    ASTNode *container;
public:
    _ASTNode(QString name) : name(name), container(NULL) {}
    void setContainer(ASTNode * container) {this->container = container;}
    ASTNode &addChild(ASTNode child) {children.append(child);return *container;}
    ASTNode &addData(QString name, QString data) {strdata[name]=data; return *container;}
};
#define new_ASTNode(name) ASTNode(new _ASTNode(name))


#endif // IONPARSERLIB_H
