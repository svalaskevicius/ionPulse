/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "phpparser.h"

#include <iostream>
#include <QVector>

#include "phpParser/gen_php_parser.hpp"
#include <QFile>
#include <QFileInfo>
#include <QDateTime>

extern int _impl_ionPhp_lex(IonDbXml::XmlNode **astNode, yyscan_t yyscanner);

extern int ion_php_parse(IonPhp::Private::PhpParser* context);


namespace IonPhp {
namespace Private {


PhpParser::PhpParser()
{
    init_scanner();
}

PhpParser::~PhpParser()
{
    destroy_scanner();
}

IonPhp::Private::ParserResult *PhpParser::parseString(QString doc)
{
    void * buf = setBuf(doc.toLatin1().constData());
    __result = NULL;
    ParserResult *result = new ParserResult();
    result->success = true;
    int ret;
    try {
        ret = ion_php_parse(this);
    } catch (std::exception& e) {
        result->error.lineFrom = __line;
        result->error.colFrom = __col;
        result->error.lineTo = __posLine;
        result->error.colTo = __posCol;
        result->error.message = QString("exception while parsing: %1 at: %2,%3 : %4,%5")
                        .arg(e.what())
                        .arg(__line).arg(__col)
                        .arg(__posLine).arg(__posCol)
                    ;
        result->success = false;
    }
    delBuf(buf);

    result->setRoot(__result);

    return result;
}

IonPhp::Private::ParserResult *PhpParser::parseFile(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return parseString(file.readAll());
    }
    ParserResult *result = new ParserResult();
    result->success = false;
    result->error.message = "parsing of the file failed: file not found: "+path;
    return result;
}

void PhpParser::__error(PhpParser *myself, const char *error) {
    Q_ASSERT(this == myself);
    throw std::logic_error(error);
}

int PhpParser::__lex(IonDbXml::XmlNode **astNode, yyscan_t yyscanner)
{
    IonDbXml::XmlNode *lastNode = *astNode;
    while(1) {
        *astNode = NULL;
        int ret = _impl_ionPhp_lex(astNode, yyscanner);
        switch (ret) {
            case T_COMMENT:
            case T_DOC_COMMENT:
            case T_OPEN_TAG:
            case T_WHITESPACE:
                if (*astNode) {
                    ASTNode::destroy(*astNode);
                }
                continue;

            case T_CLOSE_TAG:
                  return ';'; /* implicit ; */
            case T_OPEN_TAG_WITH_ECHO:
                  return T_ECHO;
            case T_INLINE_HTML:
                if (lastNode && (lastNode->getName() == "inline_html")) {
                    // join them
                    lastNode->setText(lastNode->getText()+(*astNode)->getText());
                    ASTNode::destroy(*astNode);
                    continue;
                }
            default:
                return ret;
        }
    }
}
/**
 * invoked by scanner on failure to match tokens
 */
void PhpParser::__echo(const char *text, int size)
{
    QString txt;
    for (int i=0;i<size;i++) {
        txt += text[i];
    }
    throw std::logic_error("failed to match in scanner: "+txt.toStdString());
}

}
}
