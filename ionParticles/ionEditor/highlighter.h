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
    explicit Highlighter(QPlainTextEdit *parent = 0);

protected:
    void highlightBlock(const QString &);

private:
    QPlainTextEdit *ionText;
};

}


#endif // IONHIGHLIGHTER_H
