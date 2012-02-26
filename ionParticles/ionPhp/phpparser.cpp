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

extern int _impl_ionPhp_lex(IonPhp::Private::pASTNode *astNode, yyscan_t yyscanner);

extern int ion_php_parse(IonPhp::Private::phpParser* context);


namespace IonPhp {
namespace Private {


phpParser::phpParser()
{
    init_scanner();
}

phpParser::~phpParser()
{
    destroy_scanner();
}

QSharedPointer<ASTRoot> phpParser::parseString(QString doc)
{
    void * buf = setBuf(doc.toAscii().constData());
    __result = NULL;
    int ret = 1;
    QString errorText = "unknown error";
    try {
        ret = ion_php_parse(this);
    } catch (std::exception& e) {
        errorText = QString("exception while parsing: %1 at: %2,%3 : %4,%5")
                        .arg(e.what())
                        .arg(__line).arg(__col)
                        .arg(__posLine).arg(__posCol)
                    ;
    }
    delBuf(buf);

    if (ret || !__result) {
        if (__result) {
            delete __result;
            __result = NULL;
        }
        throw std::runtime_error(errorText.toStdString());
    }

    return QSharedPointer<ASTRoot>(new ASTRoot(__result));
}

QSharedPointer<ASTRoot> phpParser::parseFile(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        try {
            return parseString(file.readAll());
        } catch (std::runtime_error &err) {
            throw std::runtime_error("parsing of the file failed: file not found: "+path.toStdString()+"\n"+err.what());
        }
    }
    throw std::runtime_error("parsing of the file failed: file not found: "+path.toStdString());
}

void phpParser::__error(phpParser *myself, const char *error) {
    Q_ASSERT(this == myself);
    throw std::logic_error(error);
}

int phpParser::__lex(pASTNode *astNode, yyscan_t yyscanner)
{
    pASTNode lastNode = *astNode;
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
void phpParser::__echo(const char *text, int size)
{
    QString txt;
    for (int i=0;i<size;i++) {
        txt += text[i];
    }
    throw std::logic_error("failed to match in scanner: "+txt.toStdString());
}

}
}
