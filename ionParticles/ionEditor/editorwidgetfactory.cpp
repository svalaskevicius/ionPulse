#include "editorwidgetfactory.h"
#include "ionHeart/shared.h"


namespace IonEditor {

LineNumberArea *EditorWidgetFactory::LineNumberArea::operator ()(EditorWidget *widget)
{
    return new IonEditor::LineNumberArea(widget);
}

Highlighter *EditorWidgetFactory::Highlighter::operator ()(EditorWidget *widget)
{
    return new IonEditor::Highlighter(widget);
}

Highlighter *EditorWidgetFactory::createHighlighter(EditorWidget *widget, QString filetype)
{
    QMap<QString, QSharedPointer<Highlighter> >::const_iterator it = m_createHighlighterMap.find(filetype);
    if (it != m_createHighlighterMap.end()) {
        return (*(*it))(widget);
    }
    Highlighter _default;
    return _default(widget);
}
LineNumberArea *EditorWidgetFactory::createLineNumberArea(EditorWidget *widget, QString filetype)
{
    QMap<QString, QSharedPointer<LineNumberArea> >::const_iterator it = m_createLineNumberAreaMap.find(filetype);
    if (it != m_createLineNumberAreaMap.end()) {
        return (*(*it))(widget);
    }
    LineNumberArea _default;
    return _default(widget);
}

}
