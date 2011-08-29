#include "editorwidget.h"
#include <ionHeart/shared.h>
#include "linenumberarea.h"
#include "highlighter.h"
#include "editorwidgetfactory.h"
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

namespace IonEditor {


EditorWidget::EditorWidget(QString filePath)
    :
      QPlainTextEdit(NULL),
      highlighter(NULL),
      filePath(filePath)
{
    QFont font("Monaco");
    font.setStyleHint(QFont::Courier, QFont::PreferAntialias);
    document()->setDefaultFont(font);
    QFile f(filePath);
    if (f.open(QFile::ReadOnly)) {
        setPlainText(QTextStream(&f).readAll());
    }
}

EditorWidget::~EditorWidget()
{
    if (highlighter) {
        delete highlighter;
    }
    resetComponents();
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


void EditorWidget::resetComponents() {
    foreach (Component *component, components) {
        delete component;
    }
    components.clear();
}
void EditorWidget::resetHighlighter() {
    if (highlighter) {
        delete highlighter;
    }
}

QString EditorWidget::getPanelTitle() {
    return QFileInfo(filePath).fileName();
}


}
