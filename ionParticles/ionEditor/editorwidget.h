#ifndef IONTEXTEDITORWIDGET_H
#define IONTEXTEDITORWIDGET_H

#include <QPlainTextEdit>
#include "highlighter.h"

namespace IonEditor {


class EditorWidget : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit EditorWidget(QWidget *parent = 0);
    virtual ~EditorWidget();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
    IonHighlighter *highlighter;

signals:

public slots:

};


}


#endif // IONTEXTEDITORWIDGET_H
