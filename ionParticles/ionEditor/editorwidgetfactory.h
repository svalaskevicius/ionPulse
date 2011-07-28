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
    LineNumberArea *createLineNumerArea(EditorWidget *);
    Highlighter *createHighlighter(EditorWidget *);
};

}

#endif // EDITORWIDGETFACTORY_H
