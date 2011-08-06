#include "phpparser.h"
#include "phpParser/output/ionPhpLexer.h"
#include "phpParser/output/ionPhpParser.h"

#include <iostream>
#include <QVector>

namespace IonPhp {


phpParser::phpParser()
{
}

void print_tree (pANTLR3_BASE_TREE node, int level = 0) {
    if (!node) {
        return;
    }
    if (node->toString) {
        std::cout << std::string((char *)node->toString(node)->chars);
    } else {
        std::cout << " NIL ";
    }
    std::cout << "; ";
    if (int count = node->getChildCount(node)) {
        std::cout << " ( ";
        for (int i=0; i < count; i++) {
            print_tree((pANTLR3_BASE_TREE)node->getChild(node, i));
        }
        std::cout << " ) ";
    }
}

bool phpParser::parse(QString doc, QString name)
{
    return false;
}

}
