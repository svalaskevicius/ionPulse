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

#include "phpParser/gen_php_parser.hpp"

typedef void * yyscan_t;

namespace IonPhp {
namespace Private {

class PhpParser : public QObject
{
    Q_OBJECT
public:
    PhpParser();
    ~PhpParser();
    Q_INVOKABLE IonPhp::Private::ParserResult *parseString(QString doc);
    Q_INVOKABLE IonPhp::Private::ParserResult *parseFile(QString path);
protected:
   void init_scanner();
   void destroy_scanner();
   void *setBuf(const char*);
   void delBuf(void *);
public:
    void* __scanner;
    ParserResult *__result;
    int __line, __col, __posLine, __posCol;
    QList<int> __posColHistory;
    void __error(YYLTYPE const * const yylocationp, PhpParser *myself, const char *error);
    int  __lex(IonDbXml::XmlNode **astNode, YYLTYPE * yylocationp, yyscan_t yyscanner);
    void __echo(const char *text, int size);
    QString currentHeredocLabel;
};

}
}

#endif // PHPPARSER_H
