#include "editorwidget.h"
#include <ionHeart/shared.h>
#include "linenumberarea.h"
#include "highlighter.h"
#include "editorwidgetfactory.h"


namespace IonEditor {


EditorWidget::EditorWidget(EditorWidgetFactory *widgetFactory, QWidget *parent) : QPlainTextEdit(parent), widgetFactory(widgetFactory)
{
    lineNumberArea = widgetFactory->createLineNumberArea(this);
    addComponent(lineNumberArea);

    highlighter = widgetFactory->createHighlighter(this);

    QFont font("Monaco");
    font.setStyleHint(QFont::Courier, QFont::PreferAntialias);
    document()->setDefaultFont(font);

    updateViewportMargins();
}

EditorWidget::~EditorWidget()
{
    if (highlighter) {
        delete highlighter;
    }
    if (lineNumberArea) {
        delete lineNumberArea;
    }
}





void EditorWidget::updateViewportMargins()
{
    QMargins margins(0, 0, 0, 0);
    foreach (Component *component, components) {
        margins.setLeft(margins.left()+component->getWidth());
    }

    setViewportMargins(margins);
}


bool EditorWidget::event ( QEvent * event ) {
    foreach (Component *component, eventListeners[event->type()]) {
        component->editorEvent(event);
    }
    return QPlainTextEdit::event(event);
}



}
