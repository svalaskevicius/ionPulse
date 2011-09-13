/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "ionParserLib.h"
#include <iostream>


#define appendSpaces(w, l) { for(int _i=l;_i>0;_i--) w+=" ";}


QString ASTNode::toMlString(int indentLevel)
{
    QString ret;
    if (indentLevel > 0) {
        appendSpaces(ret, indentLevel*4);
    }
    ret += name;
    if (strdata.count()) {
        ret += " [";
        QMap<QString, QString>::const_iterator i = strdata.begin();
        bool _1st = true;
        while (i != strdata.end()) {
            if (!_1st) {
                 ret += ", ";
            } else {
                _1st = false;
            }
            ret += i.key() + ":" + i.value();
            ++i;
        }
        ret += "]";
    }
    if (children.size()) {
        ret += "(";
        if (indentLevel >= 0) {
            ret += "\n";
        }
        bool _1st = true;
        foreach (pASTNode child, children) {
            if (!_1st) {
                 if (indentLevel >= 0) {
                     ret += "\n";
                 } else {
                     ret += "; ";
                 }
            } else {
                _1st = false;
            }
            ret += child->toMlString(indentLevel<0?indentLevel:indentLevel+1);
        }
        if (indentLevel >= 0) {
            ret += "\n";
            appendSpaces(ret, indentLevel*4);
        }
        ret += ")";
    }
    return ret;
}
