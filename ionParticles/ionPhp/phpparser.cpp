#include "phpparser.h"

#include <iostream>
#include <QVector>

#include "gen_php_parser.hpp"

extern int _impl_ionPhp_lex(pASTNode *astNode, yyscan_t yyscanner);

int ion_php_parse(IonPhp::phpParser* context);


namespace IonPhp {


phpParser::phpParser()
{
    init_scanner();
}

phpParser::~phpParser()
{
    destroy_scanner();
}

/*void print_tree (pANTLR3_BASE_TREE node, int level = 0) {
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
*/

bool phpParser::parse(QString doc)
{
    void * buf = setBuf(doc.toAscii().constData());
    __result = NULL;
    int ret = ion_php_parse(this);
    delBuf(buf);

    //std::cout << ret << std::endl;

    if (!ret && __result) {
        // OK.
        __result->print_r();
        return true;
    }

    if (!ret && !__result) {
        std::cerr << "DBG@\n";
    }

    return false;
}


void phpParser::__error(phpParser *myself, const char *error) {
    Q_ASSERT(this == myself);
    std::cerr << "error: " << error << "\n";
}

int  phpParser::__lex(pASTNode *astNode, yyscan_t yyscanner)
{
    while(1) {
        *astNode = NULL;
        int ret = _impl_ionPhp_lex(astNode, yyscanner);
        switch (ret) {
            case T_COMMENT:
            case T_DOC_COMMENT:
            case T_OPEN_TAG:
            case T_WHITESPACE:
                  continue;

            case T_CLOSE_TAG:
                  return ';'; /* implicit ; */
            case T_OPEN_TAG_WITH_ECHO:
                  return T_ECHO;
            default:
//                std::cout << "TOK: " << ret << std::endl;
                  return ret;
        }
    }
}

}
