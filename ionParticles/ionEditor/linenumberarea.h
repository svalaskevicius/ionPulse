#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>
#include "editorwidget.h"

namespace IonEditor {


class LineNumberArea : public QWidget, public EditorWidget::Component
{
    Q_OBJECT
public:
    explicit LineNumberArea(EditorWidget *parent = 0);
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
    EditorWidget *ionText;
    uint currentLine;
};

}

#endif // LINENUMBERAREA_H
