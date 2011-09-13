/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

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

ASTRoot phpParser::parse(QString doc)
{
    void * buf = setBuf(doc.toAscii().constData());
    __result = NULL;
    int ret = ion_php_parse(this);
    delBuf(buf);

    //std::cout << ret << std::endl;

    if (ret) {
        if (__result) {
            delete __result;
            __result = NULL;
        }
    }

    return ASTRoot(__result);
}


void phpParser::__error(phpParser *myself, const char *error) {
    Q_ASSERT(this == myself);
    std::cerr << "error: " << error << "\n";
}

int  phpParser::__lex(pASTNode *astNode, yyscan_t yyscanner)
{
    while(1) {
        pASTNode lastNode = *astNode;
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
            case T_INLINE_HTML:
                if (lastNode && (lastNode->getName() == "T_INLINE_HTML")) {
                    // join them
                    lastNode->setData("text", lastNode->getStrData("text")+(*astNode)->getStrData("text"));
                    continue;
                }
            default:
/*                std::cout << "TOK: " << ret;
                if (*astNode) {
                    std::cout << " "  << (*astNode)->getStrData("text").toStdString();
                }
                std::cout << std::endl;
  */                return ret;
        }
    }
}

}
