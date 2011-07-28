#ifndef IONHIGHLIGHTER_H
#define IONHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>

namespace IonEditor {

class EditorWidget;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit Highlighter(EditorWidget *parent = 0);

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


private:
    EditorWidget *ionText;
private slots:
    void editorCursorPositionChanged();
protected:
    void addCurrentLineExtraSelection(QList<QTextEdit::ExtraSelection> &extraSelections);
};

}


#endif // IONHIGHLIGHTER_H
