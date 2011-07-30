#ifndef PHPHIGHLIGHTER_H
#define PHPHIGHLIGHTER_H

#include <ionParticles/ionEditor/highlighter.h>

namespace IonPhp {

class Highlighter : public IonEditor::Highlighter
{
    Q_OBJECT
public:
    explicit Highlighter(IonEditor::EditorWidget *parent);
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

protected:
    void highlightBlock(const QString &text);


signals:

public slots:

};

}

#endif // PHPHIGHLIGHTER_H
