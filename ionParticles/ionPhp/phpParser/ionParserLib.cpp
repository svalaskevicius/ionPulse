/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "ionParserLib.h"
#include <iostream>

namespace IonPhp {
namespace Private {

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

ASTNode::ASTNode(QString name)
    :  name(name), lineNr(-1), columnNr(-1)
{
}

QString ASTNode::toString() {
    return ast2xml()(this, 2);
}

pASTNode ASTNode::setPosition(int lineNr, int columnNr)
{
    this->lineNr = lineNr;
    this->columnNr = columnNr;
    attributes["lineNr"] = QString("%1").arg(lineNr);
    attributes["columnNr"] = QString("%1").arg(columnNr);
    return this;
}

int ASTNode::getLine() const
{
    return lineNr;
}

int ASTNode::getColumn() const
{
    return columnNr;
}

pASTNode ASTNode::addChild(pASTNode child)
{
    if (!child) {
        throw std::invalid_argument( "child must be set" );
    }
    child->setParent(this);
    children.append(child);
    return this;
}

pASTNode ASTNode::setData(QString name, QString data)
{
    attributes[name] = data;
    return this;
}

pASTNode ASTNode::setText(QString data)
{
    text = data;
    return this;
}

QString ASTNode::getName()
{
    return name;
}

QString ASTNode::getData(QString name)
{
    return attributes[name];
}

QString ASTNode::getText()
{
    return text;
}

pASTNode ASTNode::create(QString name)
{
    return new ASTNode(name);
}
void ASTNode::destroy(pASTNode node)
{
    delete node;
}



ASTRoot::ASTRoot(pASTNode rootNode)
    : rootNode(rootNode)
{
}

ASTRoot::~ASTRoot()
{
    delete rootNode;
}

}
}
