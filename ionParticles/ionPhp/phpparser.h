/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PHPPARSER_H
#define PHPPARSER_H

#include <QString>
#include "phpParser/ionParserLib.h"

typedef void * yyscan_t;

namespace IonPhp {

class phpParser
{
public:
    phpParser();
    ~phpParser();
    ASTRoot parse(QString doc);
protected:
   void init_scanner();
   void destroy_scanner();
   void *setBuf(const char*);
   void delBuf(void *);
public:
    void* __scanner;
    pASTNode __result;
    void __error(phpParser *myself, const char *error);
    int  __lex(pASTNode *astNode, yyscan_t yyscanner);
    QString currentHeredocLabel;
};

}

#endif // PHPPARSER_H
