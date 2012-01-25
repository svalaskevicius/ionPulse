/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PHPPARSER_H
#define PHPPARSER_H

#include <QString>
#include <QList>
#include "phpParser/ionParserLib.h"

typedef void * yyscan_t;

namespace IonPhp {

class phpParser
{
public:
    phpParser();
    ~phpParser();
    QSharedPointer<ASTRoot> parseString(QString doc);
    QSharedPointer<ASTRoot> parseFile(QString path);
protected:
   void init_scanner();
   void destroy_scanner();
   void *setBuf(const char*);
   void delBuf(void *);
public:
    void* __scanner;
    pASTNode __result;
    int __line, __col, __posLine, __posCol;
    QList<int> __posColHistory;
    void __error(phpParser *myself, const char *error);
    int  __lex(pASTNode *astNode, yyscan_t yyscanner);
    void __echo(const char *text, int size);
    QString currentHeredocLabel;
};

}

#endif // PHPPARSER_H
