#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include "linenumberarea.h"
#include "highlighter.h"
#include <QSharedPointer>


namespace IonEditor {

class EditorWidget;

struct EditorWidgetFactory
{
protected:
    struct LineNumberArea {
        virtual IonEditor::LineNumberArea *operator()(EditorWidget *);
    };
    struct Highlighter {
        virtual IonEditor::Highlighter *operator()(EditorWidget *);
    };
private:
    QMap<QString, QSharedPointer<Highlighter> > m_createHighlighterMap;
    QMap<QString, QSharedPointer<LineNumberArea> > m_createLineNumberAreaMap;

public:
    IonEditor::Highlighter *createHighlighter(EditorWidget *widget, QString filetype);
    IonEditor::LineNumberArea *createLineNumberArea(EditorWidget *widget, QString filetype);

    void registerHighlighter(QString filetype, Highlighter *highlighter) {
        m_createHighlighterMap[filetype] = QSharedPointer<Highlighter>(highlighter);
    }
    void registerLineNumberArea(QString filetype, LineNumberArea *lineNumberArea) {
        m_createLineNumberAreaMap[filetype] = QSharedPointer<LineNumberArea>(lineNumberArea);
    }
};

}

#endif // EDITORWIDGETFACTORY_H
