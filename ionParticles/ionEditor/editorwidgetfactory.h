#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include "linenumberarea.h"
#include "highlighter.h"


namespace IonEditor {

class EditorWidget;

class EditorWidgetFactory
{
public:
    EditorWidgetFactory();
    struct LineNumberArea {
        IonEditor::LineNumberArea *operator()(EditorWidget *);
    };
    struct Highlighter {
        IonEditor::Highlighter *operator()(EditorWidget *);
    };
    Highlighter createHighlighter;
    LineNumberArea createLineNumerArea;
};

}

#endif // EDITORWIDGETFACTORY_H
