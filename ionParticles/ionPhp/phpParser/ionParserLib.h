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
    Q_OBJECT
private:
    QVector<XmlNode*> children;
    AttributesMap attributes;
    QString text;
    QString name;
    int lineNr, columnNr;
public:
    ASTNode(QString name);
    ~ASTNode();
    Q_INVOKABLE pASTNode setPosition(int lineNr, int columnNr);
    Q_INVOKABLE int getLine() const;
    Q_INVOKABLE int getColumn() const;
    Q_INVOKABLE pASTNode addChild(pASTNode child);
    Q_INVOKABLE pASTNode setData(QString name, QString data);
    Q_INVOKABLE pASTNode setText(QString data);
    Q_INVOKABLE QString getData(QString name);

    Q_INVOKABLE QString getName();
    Q_INVOKABLE QString getText();
    Q_INVOKABLE AttributesMap &getAttributes() { return attributes;}
    Q_INVOKABLE QVector<IonDbXml::XmlNode*> &getChildren() {return children;}

    Q_INVOKABLE QString toString();

    static pASTNode create(QString name);
    static void destroy(pASTNode node);
};

class ASTRoot : public QObject {
    Q_OBJECT
protected:
    pASTNode rootNode;
public:
    ASTRoot(pASTNode rootNode);
    ~ASTRoot();

    Q_INVOKABLE IonPhp::Private::ASTNode *getRootNode() {
        return rootNode;
    }

    Q_INVOKABLE QString toString() {
        return getRootNode()->toString();
    }
};

class ParserError : public QObject {
    Q_OBJECT
public:
    QString message;
    int lineFrom, colFrom, lineTo, colTo;

    Q_INVOKABLE QString getMessage() {
        return message;
    }
    Q_INVOKABLE int getLineFrom() {
        return lineFrom;
    }
    Q_INVOKABLE int getColFrom() {
        return colFrom;
    }
    Q_INVOKABLE int getLineTo() {
        return lineTo;
    }
    Q_INVOKABLE int getColTo() {
        return colTo;
    }
};

class ParserResult : public QObject {
    Q_OBJECT
public:
    bool success;
    QSharedPointer<ASTRoot> root;
    ParserError error;

    Q_INVOKABLE IonPhp::Private::ASTRoot* getRoot() {
        return root.data();
    }
    Q_INVOKABLE bool getSuccess() {
        return success;
    }
    Q_INVOKABLE IonPhp::Private::ParserError* getError() {
        return &error;
    }
};

}
}

#endif // IONPARSERLIB_H
