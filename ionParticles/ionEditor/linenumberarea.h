#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>
#include "editorwidget.h"

namespace IonEditor {

namespace Private {

class LineNumberArea : public QWidget, public IEditorComponent
{
    Q_OBJECT
public:
    explicit LineNumberArea(IEditor *parent = 0);
    virtual ~LineNumberArea(){}
    QSize sizeHint() {
        return QSize(getWidth(), 0);
    }
    int getWidth();
    void editorEvent(QEvent * event);
protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private slots:
    void editorUpdateRequest(const QRect &, int);
    void editorBlockCountChanged(int /* newBlockCount */);
    void editorCursorPositionChanged();
private:
    IEditor *ionText;
    int currentLine;
};

}

}

#endif // LINENUMBERAREA_H
