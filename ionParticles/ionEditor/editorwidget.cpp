#include "editorwidget.h"
#include <ionPulse/shared.h>
#include "linenumberarea.h"
#include "highlighter.h"

namespace IonEditor {


EditorWidget::EditorWidget(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    addComponent(lineNumberArea);

    highlighter = new Highlighter(this);

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
