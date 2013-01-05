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

    struct ast2xml {
        QString operator()(IonDbXml::XmlNode * node, int indent = 0)
        {
            QString ws = "  ";
            QString ret = ws.repeated(indent) + "<" + node->getName();
            for (IonPhp::Private::ASTNode::AttributesMap::const_iterator it = node->getAttributes().begin(); it != node->getAttributes().end(); it++) {
                ret += QString(" %1=\"%2\"").arg(it.key()).arg(it.value());
            }

            bool content = false;
            if (node->getText().length()) {
                ret += ">";
                QString t = node->getText();
                ret += t.replace("\r", "&#xD;").replace("<", "&lt;").replace(">", "&gt;");
                content = true;
            }
            if (node->getChildren().count()) {
                if (!content) {
                    ret += ">\n";
                }
                foreach (IonDbXml::XmlNode * child, node->getChildren()) {
                    ret += this->operator ()(child, indent + 1);
                }
                ret += ws.repeated(indent);
                content = true;
            }
            if (!content) {
                ret += "/>\n";
            } else {
                ret += "</" + node->getName() + ">\n";
            }

            return ret;
        }
    };

    ASTRoot(pASTNode rootNode);
    ~ASTRoot();
    pASTNode getRootNode() {
        return rootNode;
    }

    QString toString() {
        return ast2xml()(getRootNode(), 2);
    }
};

struct ParserResult {
    struct Error {
        QString message;
        int lineFrom, colFrom, lineTo, colTo;
    };
    bool success;
    QSharedPointer<ASTRoot> root;
    Error error;
};

}
}

#endif // IONPARSERLIB_H
