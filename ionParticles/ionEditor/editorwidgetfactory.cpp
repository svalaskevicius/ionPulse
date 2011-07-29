#include "editorwidgetfactory.h"
#include "ionHeart/ionPulse/shared.h"


namespace IonEditor {

LineNumberArea *EditorWidgetFactory::LineNumberArea::operator ()(EditorWidget *widget)
{
    return new IonEditor::LineNumberArea(widget);
}

Highlighter *EditorWidgetFactory::Highlighter::operator ()(EditorWidget *widget)
{
    return new IonEditor::Highlighter(widget);
}

Highlighter *EditorWidgetFactory::createHighlighter(EditorWidget *widget)
{
    if (!m_createHighlighter) {
        DEBUG_MSG("m_createHighlighter.reset");
        m_createHighlighter.reset(new Highlighter());
    }
    return (*m_createHighlighter)(widget);
}
LineNumberArea *EditorWidgetFactory::createLineNumberArea(EditorWidget *widget)
{
    if (!m_createLineNumberArea) {
        m_createLineNumberArea.reset(new LineNumberArea());
    }
    return (*m_createLineNumberArea)(widget);
}

}
