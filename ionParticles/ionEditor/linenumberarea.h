#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>
#include "iontexteditorwidget.h"

class LineNumberArea : public QWidget
{
    Q_OBJECT
public:
    explicit LineNumberArea(IonTextEditorWidget *parent = 0);
    QSize sizeHint() const {
        return QSize(ionText->lineNumberAreaWidth(), 0);
    }
protected:
     void paintEvent(QPaintEvent *event) {
         ionText->lineNumberAreaPaintEvent(event);
     }

signals:

public slots:

private:
    IonTextEditorWidget *ionText;
};

#endif // LINENUMBERAREA_H
