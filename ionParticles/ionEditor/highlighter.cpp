#include "highlighter.h"
#include <ionHeart/shared.h>
#include "editorwidget.h"

namespace IonEditor {

Highlighter::Highlighter(EditorWidget *parent) :
    QSyntaxHighlighter(parent->document()), ionText(parent)
{

    connect(ionText, SIGNAL(cursorPositionChanged()), this, SLOT(editorCursorPositionChanged()));
    editorCursorPositionChanged();

}

void Highlighter::highlightBlock(const QString &)
{
}




void Highlighter::editorCursorPositionChanged()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    addCurrentLineExtraSelection(extraSelections);

    ionText->setExtraSelections(extraSelections);
}

void Highlighter::addCurrentLineExtraSelection(QList<QTextEdit::ExtraSelection> &extraSelections)
{
    if (!ionText->isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(QColor::fromHsl(90, 70, 220));

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = ionText->textCursor();
        selection.cursor.clearSelection();

        extraSelections.append(selection);
    }
}

}
