#ifndef PHPHIGHLIGHTER_H
#define PHPHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>

namespace IonPhp {

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit Highlighter(QPlainTextEdit *parent);
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat multiLineCommentFormat;

protected:
    virtual void highlightBlock(const QString &text);


signals:

public slots:

};

}

#endif // PHPHIGHLIGHTER_H
