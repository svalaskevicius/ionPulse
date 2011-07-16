#ifndef IONHIGHLIGHTER_H
#define IONHIGHLIGHTER_H

#include <QSyntaxHighlighter>

namespace IonEditor {


class IonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit IonHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;

};

}


#endif // IONHIGHLIGHTER_H
