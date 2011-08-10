#include "ionParserLib.h"
#include <iostream>


#define coutSpaces(l) { for(int _i=l;_i>0;_i--) std::cout << " ";}


void ASTNode::_print_r(int level)
{
    coutSpaces(level*4);
    using namespace std;
    cout << name.toStdString();
    if (strdata.count()) {
        cout << " [";
        QMap<QString, QString>::const_iterator i = strdata.begin();
        bool _1st = true;
        while (i != strdata.end()) {
            if (!_1st) {
                 cout << ", ";
            } else {
                _1st = false;
            }
            cout << i.key().toStdString() << ":" << i.value().toStdString();
            ++i;
        }
        cout << "]";
    }
    cout << "(\n";
    foreach (pASTNode child, children) {
        child->_print_r(level+1);
    }
     coutSpaces(level*4);
     cout << ")\n";
}


QString ASTNode::toString()
{
    QString ret = name;
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
        bool _1st = true;
        foreach (pASTNode child, children) {
            if (!_1st) {
                 ret += "; ";
            } else {
                _1st = false;
            }
            ret += child->toString();
        }
        ret += ")";
    }
    return ret;
}
