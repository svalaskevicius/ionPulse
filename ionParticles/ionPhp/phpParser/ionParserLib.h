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

#define YYSTYPE IonDbXml::XmlNode*

namespace IonPhp {
namespace Private {

class ASTNode;

class ASTNode : public IonDbXml::XmlNode {
private:
    QList<IonDbXml::XmlNode*> children;
    AttributesMap attributes;
    QString text;
    QString name;
public:
    ASTNode(QString name);
    IonDbXml::XmlNode* setStartPosition(int lineNr, int columnNr);
    IonDbXml::XmlNode* setEndPosition(int lineNr, int columnNr);
    int getStartLine();
    int getEndLine();
    int getStartCol();
    int getEndCol();

    IonDbXml::XmlNode* addChild(IonDbXml::XmlNode* child);
    IonDbXml::XmlNode* setData(QString name, QVariant data);
    IonDbXml::XmlNode* setText(QString data);
    QVariant getData(QString name);

    QString getName();
    QString getText();

    AttributesMap &getAttributes() { return attributes;}
    QList<IonDbXml::XmlNode*> &getChildren() {return children;}

    QString toString();

    static IonDbXml::XmlNode* create(QString name);
    static void destroy(IonDbXml::XmlNode* node);
};


class ParserError : public QObject {
    Q_OBJECT
public:
    QString message;
    int lineFrom, colFrom, lineTo, colTo;
    int repeatedTimes;

    ParserError() {repeatedTimes = 0;}
    ParserError(const ParserError& src)
        : message(src.message),
          lineFrom(src.lineFrom), colFrom(src.colFrom),
          lineTo(src.lineTo), colTo(src.colTo), repeatedTimes(src.repeatedTimes)
    {}

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
    bool operator==(const ParserError &other) {
        return (lineFrom == other.lineFrom)
            && (colFrom == other.colFrom)
            && (lineTo == other.lineTo)
            && (colTo == other.colTo)
            && (message == other.message)
        ;
    }
};

class ParserResult : public QObject {
    Q_OBJECT
private:
    IonDbXml::XmlNode* root;
public:
    bool success;
    QList<ParserError> errors;

    Q_INVOKABLE void setRoot(IonDbXml::XmlNode* newRoot) {
        root = newRoot;
        if (root) {
            root->setParent(this);
        }
    }
    Q_INVOKABLE IonDbXml::XmlNode* getRoot() {
        return root;
    }
    Q_INVOKABLE bool getSuccess() {
        return success;
    }
};

}
}

#endif // IONPARSERLIB_H
