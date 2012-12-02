/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "linenumberarea.h"
#include <QPainter>
#include <QTextBlock>
#include <QPaintEvent>
#include <ionCore/shared.h>

namespace IonEditor {
namespace Private {

LineNumberArea::LineNumberArea(Editor *parent) :
    QWidget(parent),
    ionText(parent),
    currentLine(0)
{
    ionText->addEventListener(QEvent::Resize, this);
    connect(ionText, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(editorUpdateRequest(const QRect &, int)));
    connect(ionText, SIGNAL(blockCountChanged(int)), this, SLOT(editorBlockCountChanged(int)));
    connect(ionText, SIGNAL(zoomRatioChanged(float)), this, SLOT(editorZoomRatioChanged(float)));
    connect(ionText, SIGNAL(cursorPositionChanged()), this, SLOT(editorCursorPositionChanged()));

    editorBlockCountChanged(0);
}


void LineNumberArea::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    QColor bg = palette().color(QPalette::Background);
    painter.fillRect(event->rect(), bg);

    QTextBlock block = ionText->getEditorInfo().firstVisibleBlock();
    int blockNumber = block.blockNumber();
    double top = ionText->getEditorInfo().blockBoundingGeometry(block).translated(ionText->getEditorInfo().contentOffset()).top();

    painter.setFont(ionText->document()->defaultFont());
    double fontHeight = painter.fontMetrics().height();

    bool resetColor = true;
    while (block.isValid() && top <= event->rect().bottom()) {
        double blockHeight = ionText->getEditorInfo().blockBoundingRect(block).height();
        double bottom = top + blockHeight;

        if (block.isVisible() && bottom >= event->rect().top()) {
            if (blockNumber == currentLine) {
                painter.setPen(palette().color(QPalette::Foreground).lighter(120));
                resetColor = true;
            } else if (resetColor) {
                painter.setPen(palette().color(QPalette::Foreground));
                resetColor = false;
            }

            painter.drawText(
                QRectF(
                    3,
                    top + (blockHeight / (double)block.lineCount() - fontHeight) / 2.0f,
                    this->width()-5,
                    blockHeight
                ),
                Qt::AlignRight,
                QString::number(blockNumber + 1)
            );
        }

        block = block.next();
        top = bottom;
        ++blockNumber;
    }
    painter.end();
}


int LineNumberArea::getWidth()
{
    int digits = 1;
    int max = qMax(1, ionText->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    return 5 + ionText->fontMetrics().width(QLatin1Char('9')) * digits;
}

bool LineNumberArea::editorEvent(QEvent * event)
{
    if (QEvent::Resize == event->type()) {
        QRect cr = ionText->contentsRect();
        setGeometry(QRect(cr.left(), cr.top(), getWidth(), cr.height()));
    }
    return false;
}

void LineNumberArea::editorUpdateRequest(const QRect &rect, int dy)
{
    if (dy) {
        scroll(0, dy);
    } else {
        update(0, rect.y(), width(), rect.height());
    }

    if (rect.contains(ionText->viewport()->rect())) {
        ionText->updateViewportMargins();
    }
}

void LineNumberArea::editorBlockCountChanged(int /* newBlockCount */)
{
    resizeToFitEditor();
}

void LineNumberArea::editorZoomRatioChanged(float /* zoomRatio */)
{
    resizeToFitEditor();
}

void LineNumberArea::resizeToFitEditor()
{
    //TODO: performance: check if width changed, and only then set geom and update margins
    ionText->updateViewportMargins();
    QRect cr = ionText->contentsRect();
    setGeometry(QRect(cr.left(), cr.top(), getWidth(), cr.height()));
}

void LineNumberArea::editorCursorPositionChanged()
{
    currentLine = ionText->textCursor().blockNumber();
    update();
}

}

}
