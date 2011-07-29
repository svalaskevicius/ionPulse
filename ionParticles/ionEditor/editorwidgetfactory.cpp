#include "editorwidgetfactory.h"


namespace IonEditor {

EditorWidgetFactory::EditorWidgetFactory()
{
}

LineNumberArea *EditorWidgetFactory::LineNumberArea::operator ()(EditorWidget *widget)
{
    return new IonEditor::LineNumberArea(widget);
}

Highlighter *EditorWidgetFactory::Highlighter::operator ()(EditorWidget *widget)
{
    return new IonEditor::Highlighter(widget);
}

}
