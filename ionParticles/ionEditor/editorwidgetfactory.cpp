#include "editorwidgetfactory.h"


namespace IonEditor {

EditorWidgetFactory::EditorWidgetFactory()
{
}

LineNumberArea *EditorWidgetFactory::createLineNumerArea(EditorWidget *widget)
{
    return new LineNumberArea(widget);
}

Highlighter *EditorWidgetFactory::createHighlighter(EditorWidget *widget)
{
    return new Highlighter(widget);
}

}
