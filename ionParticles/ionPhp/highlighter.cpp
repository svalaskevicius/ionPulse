/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "highlighter.h"
#include <ionHeart/shared.h>

namespace IonPhp {

Highlighter::Highlighter(QPlainTextEdit *parent) :
    QSyntaxHighlighter(parent->document())
{
    HighlightingRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywords;
    keywords << "abstract" << "and" << "array" << "as" << "break" << "case"
            << "catch" << "class" << "clone" << "const" << "continue" << "declare"
            << "default" << "do" << "else" << "elseif" << "enddeclare" << "endfor"
            << "endforeach" << "endif" << "endswitch" << "endwhile" << "extends"
            << "final" << "for" << "foreach" << "function" << "global" << "goto"
            << "if" << "implements" << "interface" << "instanceof" << "namespace"
            << "new" << "or" << "private" << "protected" << "public" << "static"
            << "switch" << "throw" << "try" << "use" << "var" << "while" << "xor";

    foreach (const QString &pattern, keywords) {
        rule.pattern = QRegExp("\\b"+pattern+"\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    QTextCharFormat compileConstantsFormat;
    compileConstantsFormat.setForeground(Qt::blue);
    compileConstantsFormat.setFontWeight(QFont::DemiBold);
    compileConstantsFormat.setFontItalic(true);
    QStringList compileConstants;
    compileConstants << "__CLASS__" << "__DIR__" << "__FILE__" << "__LINE__"
                     << "__FUNCTION__" << "__METHOD__" << "__NAMESPACE__";
    foreach (const QString &pattern, compileConstants) {
        rule.pattern = QRegExp("\\b"+pattern+"\\b");
        rule.format = compileConstantsFormat;
        highlightingRules.append(rule);
    }

    QTextCharFormat languageConstructsFormat;
    languageConstructsFormat.setForeground(Qt::darkBlue);
    languageConstructsFormat.setFontWeight(QFont::Bold);
    QStringList languageConstructs;
    languageConstructs << "die" << "echo" << "empty" << "exit" << "eval"
                       << "include" << "include_once" << "isset" << "list"
                       << "require" << "require_once" << "return" << "print"
                       << "unset";
    foreach (const QString &pattern, languageConstructs) {
        rule.pattern = QRegExp("\\b"+pattern+"\\b");
        rule.format = languageConstructsFormat;
        highlightingRules.append(rule);
    }


    QTextCharFormat variableFormat;
    variableFormat.setForeground(QColor::fromHsv(90, 155, 85));
    rule.pattern = QRegExp("\\$[A-Za-z]+\\b");
    rule.format = variableFormat;
    highlightingRules.append(rule);

    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("([\"']).*\\1");
    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    QTextCharFormat functionFormat;
    functionFormat.setFontWeight(QFont::Bold);
    //functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor::fromHsv(20, 205, 105));
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);


    multiLineCommentFormat.setForeground(Qt::red);
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

}
