#include "editorwidget.h"
#include <ionHeart/shared.h>
#include "linenumberarea.h"
#include "highlighter.h"
#include "editorwidgetfactory.h"
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

namespace IonEditor {

//////
namespace Private {

EditorWidget::EditorWidget(QString filePath)
    :
      QPlainTextEdit(NULL),
      highlighter(NULL),
      filePath(filePath),
      componentInfo(this)
{
    QFont font("Monaco");
    font.setStyleHint(QFont::Courier, QFont::PreferAntialias);
    document()->setDefaultFont(font);
    QFile f(filePath);
    if (f.open(QFile::ReadOnly)) {
        setPlainText(QTextStream(&f).readAll());
    }
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(editorCursorPositionChanged()));
    editorCursorPositionChanged();
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
    foreach (IEditorComponent* component, components) {
        margins.setLeft(margins.left()+component->getWidth());
    }

    setViewportMargins(margins);
}


bool EditorWidget::event ( QEvent * event ) {
    foreach (IEditorComponent *component, eventListeners[event->type()]) {
        component->editorEvent(event);
    }
    return QPlainTextEdit::event(event);
}


void EditorWidget::resetComponents() {
    foreach (IEditorComponent *component, components) {
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


void EditorWidget::editorCursorPositionChanged()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    addCurrentLineExtraSelection(extraSelections);

    this->setExtraSelections(extraSelections);
}

void EditorWidget::addCurrentLineExtraSelection(QList<QTextEdit::ExtraSelection> &extraSelections)
{
    if (!this->isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(QColor::fromHsl(90, 70, 220));

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = this->textCursor();
        selection.cursor.clearSelection();

        extraSelections.append(selection);
    }
}

}
}
