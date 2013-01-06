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
            ret += QString(" %1=\"%2\"").arg(it.key()).arg(it.value().toString());
        }

        bool content = false;
        if (node->getText().length()) {
            ret += ">";
            QString t = node->getText();
            ret += t.replace("\n", "&#xA;").replace("\r", "&#xD;").replace("<", "&lt;").replace(">", "&gt;");
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
    :  name(name)
{
}

QString ASTNode::toString() {
    return ast2xml()(this, 2);
}

IonDbXml::XmlNode* ASTNode::setStartPosition(int lineNr, int columnNr)
{
    attributes["lineNr"] = lineNr;
    attributes["columnNr"] = columnNr;
    return this;
}

IonDbXml::XmlNode* ASTNode::setEndPosition(int lineNr, int columnNr)
{
    attributes["endLineNr"] = lineNr;
    attributes["endColumnNr"] = columnNr;
    return this;
}

int ASTNode::getStartLine() {
    return attributes["lineNr"].toInt();
}
int ASTNode::getEndLine() {
    return attributes["endLineNr"].toInt();
}
int ASTNode::getStartCol() {
    return attributes["columnNr"].toInt();
}
int ASTNode::getEndCol() {
    return attributes["endColumnNr"].toInt();
}


IonDbXml::XmlNode* ASTNode::addChild(IonDbXml::XmlNode* child)
{
    if (!child) {
        throw std::invalid_argument( "child must be set" );
    }
    child->setParent(this);
    children.append(child);
    return this;
}

IonDbXml::XmlNode* ASTNode::setData(QString name, QVariant data)
{
    attributes[name] = data;
    return this;
}

IonDbXml::XmlNode* ASTNode::setText(QString data)
{
    text = data;
    return this;
}

QString ASTNode::getName()
{
    return name;
}

QVariant ASTNode::getData(QString name)
{
    return attributes[name];
}

QString ASTNode::getText()
{
    return text;
}

IonDbXml::XmlNode* ASTNode::create(QString name)
{
    return new ASTNode(name);
}
void ASTNode::destroy(IonDbXml::XmlNode* node)
{
    delete node;
}



}
}
