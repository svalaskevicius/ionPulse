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
#include <ionHeart/shared.h>

namespace IonEditor {
namespace Private {

LineNumberArea::LineNumberArea(Editor *parent) :
    QWidget(parent->getEditorInstance()),
    ionText(parent),
    currentLine(0)
{
    ionText->addEventListener(QEvent::Resize, this);
    connect(ionText->getEditorInstance(), SIGNAL(updateRequest(const QRect &, int)), this, SLOT(editorUpdateRequest(const QRect &, int)));
    connect(ionText->getEditorInstance(), SIGNAL(blockCountChanged(int)), this, SLOT(editorBlockCountChanged(int)));
    connect(ionText->getEditorInstance(), SIGNAL(cursorPositionChanged()), this, SLOT(editorCursorPositionChanged()));

    editorBlockCountChanged(0);
}


void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), QColor(0xe0, 0xe0, 0xe0));

    QTextBlock block = ionText->getEditorInfo().firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) ionText->getEditorInfo().blockBoundingGeometry(block).translated(ionText->getEditorInfo().contentOffset()).top();
    int bottom = top + (int) ionText->getEditorInfo().blockBoundingRect(block).height();

    QFont font("Monaco");
    font.setStyleHint(QFont::Courier, QFont::PreferAntialias);
    font.setPixelSize(12);
    painter.setFont(font);
    bool resetColor = true;
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            if (blockNumber == currentLine) {
                painter.setPen(QColor(0x70, 0x70, 0x70));
                resetColor = true;
            } else if (resetColor) {
                painter.setPen(QColor(0xb0, 0xb0, 0xb0));
                resetColor = false;
            }

            painter.drawText(
                        QRectF(
                            3,
                            top + 1,
                            this->width()-5,
                            ionText->getEditorInfo().blockBoundingRect(block).height()
                            ),
                        Qt::AlignRight,
                        QString::number(blockNumber + 1)
                        );
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) ionText->getEditorInfo().blockBoundingRect(block).height();
        ++blockNumber;
    }
}


int LineNumberArea::getWidth()
{
    int digits = 1;
    int max = qMax(1, ionText->getEditorInstance()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 5 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void LineNumberArea::editorEvent(QEvent * event)
{
    //DEBUG_MSG((QString(">> ")+QString("%1").arg(event->type())).toStdString());
    switch(event->type()) {
    case QEvent::Resize:
    {
        QRect cr = ionText->getEditorInstance()->contentsRect();
        setGeometry(QRect(cr.left(), cr.top(), getWidth(), cr.height()));
    }
    break;
    default:
        break;
    }
}

void LineNumberArea::editorUpdateRequest(const QRect &rect, int dy)
{
    if (dy) {
        scroll(0, dy);
    } else {
        update(0, rect.y(), width(), rect.height());
    }

    if (rect.contains(ionText->getEditorInstance()->viewport()->rect())) {
        ionText->updateViewportMargins();
    }
}

void LineNumberArea::editorBlockCountChanged(int /* newBlockCount */)
{
    //DEBUG_MSG((QString(">> #")+QString("%1").arg(this->getWidth())).toStdString());
    //TODO: performance: check if width changed, and only then set geom and update margins
    ionText->updateViewportMargins();
    QRect cr = ionText->getEditorInstance()->contentsRect();
    setGeometry(QRect(cr.left(), cr.top(), getWidth(), cr.height()));
}

void LineNumberArea::editorCursorPositionChanged()
{
    currentLine = ionText->getEditorInstance()->textCursor().blockNumber();
}

}

}
