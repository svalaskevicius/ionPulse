#ifndef PHPPARSER_H
#define PHPPARSER_H

#include <QString>
#include <QScopedPointer>
#include "phpParser/ionParserLib.h"

typedef void * yyscan_t;

namespace IonPhp {

class phpParser
{
public:
    phpParser();
    ~phpParser();
    bool parse(QString doc, QString name);
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
};

}

#endif // PHPPARSER_H
