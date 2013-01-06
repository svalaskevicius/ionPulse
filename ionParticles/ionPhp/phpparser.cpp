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

struct AstEndPositionsAdjuster {
    void extendStartPos(IonDbXml::XmlNode * node, int line, int col) {
        int minLine = node->getStartLine();
        int minCol  = node->getStartCol();
        if (minLine > line) {
            node->setStartPosition(line, col);
        } else if ((minLine == line) && (minCol > col)) {
            node->setStartPosition(minLine, col);
        }
    }
    void extendEndPos(IonDbXml::XmlNode * node, int line, int col) {
        int maxLine = node->getEndLine();
        int maxCol  = node->getEndCol();
        if (maxLine < line) {
            node->setEndPosition(line, col);
        } else if ((maxLine == line) && (maxCol < col)) {
            node->setEndPosition(maxLine, col);
        }
    }
    void operator()(IonDbXml::XmlNode * node)
    {
        foreach (IonDbXml::XmlNode * subnode, node->getChildren()) {
            this->operator ()(subnode);
            this->extendEndPos(node, subnode->getEndLine(), subnode->getEndCol());
            this->extendStartPos(node, subnode->getStartLine(), subnode->getStartCol());
        }
    }
};


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
    __result = new ParserResult();
    __result->success = true;

    ion_php_parse(this);

    delBuf(buf);

    if (__result->getRoot()) {
        (AstEndPositionsAdjuster()(__result->getRoot()));
    }

    return __result;
}

IonPhp::Private::ParserResult *PhpParser::parseFile(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return parseString(file.readAll());
    }
    ParserResult *result = new ParserResult();
    result->success = false;
    ParserError error;
    error.message = "parsing of the file failed: file not found: "+path;
    result->errors.append(error);
    return result;
}

void PhpParser::__error(YYLTYPE const * const yylocationp, PhpParser *myself, const char *errorMessage) {
    Q_ASSERT(this == myself);
    __result->success = false;

    ParserError error;
    error.lineFrom = __line;
    error.colFrom = __col;
    error.lineTo = __posLine;
    error.colTo = __posCol;
    error.message = QString("exception while parsing: %1 at: %2,%3 : %4,%5")
        .arg(errorMessage)
        .arg(__line).arg(__col)
        .arg(__posLine).arg(__posCol)
    ;
    __result->errors.append(error);
}

int PhpParser::__lex(IonDbXml::XmlNode **astNode, YYLTYPE * yylocationp, yyscan_t yyscanner)
{
    IonDbXml::XmlNode *lastNode = *astNode;
    while(1) {
        *astNode = NULL;
        int ret = _impl_ionPhp_lex(astNode, yyscanner);

        yylocationp->first_line = __line;
        yylocationp->last_line = __posLine;
        yylocationp->first_column = __col;
        yylocationp->last_column = __posCol;

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
