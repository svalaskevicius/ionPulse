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

ASTNode::ASTNode(QString name)
    : lineNr(-1), columnNr(-1), name(name)
{
}

ASTNode::~ASTNode()
{
    foreach (pASTNode n, children) {
        delete n;
    }
    children.clear();
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
