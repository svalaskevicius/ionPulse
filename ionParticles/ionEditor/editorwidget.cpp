/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "editorwidget.h"
#include <ionCore/shared.h>
#include "linenumberarea.h"
#include "highlighter.h"
#include "editorwidgetfactory.h"
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <stdexcept>

namespace IonEditor {

//////
namespace Private {

EditorWidget::EditorWidget(QString filePath)
    :
      Editor(),
      highlighter(NULL),
      filePath(filePath),
      componentInfo(this)
{
    setWindowTitle(QFileInfo(filePath).fileName());
    QFont font("Monaco");
    font.setPointSize(14);
    font.setStyleHint(QFont::Courier, QFont::PreferAntialias);
    document()->setDefaultFont(font);
    setFont(font);

    QFile f(filePath);
    if (f.open(QFile::ReadOnly)) {
        setPlainText(QTextStream(&f).readAll());
    }
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(editorCursorPositionChanged()));
    connect(document(), SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)));
    document()->setModified(false);
    setWindowModified(false);
}

EditorWidget::~EditorWidget()
{
    resetHighlighter();
    resetComponents();
}





void EditorWidget::updateViewportMargins()
{
    QMargins margins(0, 0, 0, 0);
    foreach (EditorComponent* component, components) {
        margins.setLeft(margins.left()+component->getWidth());
    }

    setViewportMargins(margins);
}


bool EditorWidget::event ( QEvent * event ) {
    bool accepted = false;
    foreach (EditorComponent *component, eventListeners[event->type()]) {
        if (component->editorEvent(event)) {
            accepted = true;
        }
    }
    if (accepted) {
        return true;
    }
    return QPlainTextEdit::event(event);
}
void EditorWidget::keyPressEvent(QKeyEvent * e)
{
    switch (e->key()) {
        case Qt::Key_Backtab:
            {
                std::pair<int, int> range = getSelectedBlockRange();
                QTextBlock b = document()->findBlockByNumber(range.first);
                QTextCursor c(document());
                while ((b.blockNumber() >= 0) && (range.second >= b.blockNumber())) {
                    QString bt = b.text();
                    int indentPos = 0;
                    while((indentPos < 4)&&((0x20 == bt[indentPos]) || (0x09 == bt[indentPos]))) {
                        indentPos++;
                    }
                    c.setPosition(b.position());
                    c.setPosition(b.position()+indentPos, QTextCursor::KeepAnchor);
                    c.removeSelectedText();
                    b = b.next();
                }
            }
        break;
        case Qt::Key_Tab:
            {
                std::pair<int, int> range = getSelectedBlockRange();
                QTextBlock b = document()->findBlockByNumber(range.first);
                QTextCursor c(document());
                while ((b.blockNumber() >= 0) && (range.second >= b.blockNumber())) {
                    c.setPosition(b.position());
                    c.insertText("    ");
                    b = b.next();
                }
            }
            break;
        default:
            QPlainTextEdit::keyPressEvent(e);
    }
}

std::pair<int, int> EditorWidget::getSelectedBlockRange()
{
    QTextCursor c = textCursor();
    int selectionStart = c.selectionStart();
    int selectionEnd = c.selectionEnd();
    c.setPosition(selectionStart);
    int blockStart = c.blockNumber();
    c.setPosition(selectionEnd);
    int blockEnd = c.blockNumber();
    if (blockEnd < blockStart) {
        int tmp = blockStart;
        blockStart = blockEnd;
        blockEnd = tmp;
    }
    return std::make_pair(blockStart, blockEnd);
}

void EditorWidget::keyReleaseEvent(QKeyEvent * e)
{
    switch (e->key()) {
        case Qt::Key_Tab:
            // do nothing on release as indentation is handled on key press
            break;
        default:
            QPlainTextEdit::keyReleaseEvent(e);
    }
}


void EditorWidget::resetComponents() {
    foreach (EditorComponent *component, components) {
       delete component;
    }
    components.clear();
}
void EditorWidget::resetHighlighter() {
    if (highlighter) {
        delete highlighter;
    }
    highlighter = NULL;
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

        QColor lineColor = palette().color(QPalette::Background);
        if(lineColor.lightness() >= 192) {
            lineColor = lineColor.darker(130);
        } else {
            lineColor = lineColor.lighter(130);
        }

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = this->textCursor();
        selection.cursor.clearSelection();

        extraSelections.append(selection);
    }
}

void EditorWidget::focusOnLine(int line)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(
        document()->findBlockByNumber(line).position(),
        QTextCursor::MoveAnchor
    );
    setTextCursor(cursor);
    editorCursorPositionChanged();
}

void EditorWidget::saveFile() {
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly)) {
        throw std::runtime_error(("cannot write file "+filePath).toStdString());
    }
    f.write(document()->toPlainText().toAscii());
    document()->setModified(false);
}

void EditorWidget::modificationChanged(bool changed)
{
    if (changed) {
        setWindowTitle(QFileInfo(filePath).fileName() + " *");
    } else {
        setWindowTitle(QFileInfo(filePath).fileName());
    }
    setWindowModified(changed);
}

}
}
