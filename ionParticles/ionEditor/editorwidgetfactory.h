#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include "linenumberarea.h"
#include "highlighter.h"
#include <QScopedPointer>


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
public:
    IonEditor::Highlighter *createHighlighter(EditorWidget *widge);
    IonEditor::LineNumberArea *createLineNumberArea(EditorWidget *widge);
    QScopedPointer<Highlighter> m_createHighlighter;
    QScopedPointer<LineNumberArea> m_createLineNumberArea;
};

}

#endif // EDITORWIDGETFACTORY_H
