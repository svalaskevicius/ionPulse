#include "highlighter.h"
#include <ionHeart/shared.h>

namespace IonPhp {

Highlighter::Highlighter(IonEditor::EditorWidget *parent) :
    IonEditor::Highlighter(parent)
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
            << "switch" << "throw" << "try" << "use" << "var" << "while" << "xor"
            << "return";

    foreach (const QString &pattern, keywords) {
        rule.pattern = QRegExp("\\b"+pattern+"\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    QTextCharFormat varialeFormat;
    varialeFormat.setFontWeight(QFont::Bold);
    varialeFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\$[A-Za-z]+\\b");
    rule.format = varialeFormat;
    highlightingRules.append(rule);

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    QTextCharFormat functionFormat;
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);


    multiLineCommentFormat.setForeground(Qt::red);
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    DEBUG_MSG(text.toStdString());
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
