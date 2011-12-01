/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>
#include "editorwidget.h"

namespace IonEditor {

namespace Private {

class LineNumberArea : public QWidget, public EditorComponent
{
    Q_OBJECT
public:
    static QString identity() {return "lineNumberArea";}

    explicit LineNumberArea(Editor *parent = 0);
    virtual ~LineNumberArea(){}
    QSize sizeHint() {
        return QSize(getWidth(), 0);
    }
    int getWidth();
    bool editorEvent(QEvent * event);
protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private slots:
    void editorUpdateRequest(const QRect &, int);
    void editorBlockCountChanged(int /* newBlockCount */);
    void editorCursorPositionChanged();
private:
    Editor *ionText;
    int currentLine;
};

}

}

#endif // LINENUMBERAREA_H
