#include "linenumberarea.h"
#include <QPainter>
#include <QTextBlock>
#include <QPaintEvent>
#include <ionPulse/shared.h>

namespace IonEditor {

LineNumberArea::LineNumberArea(EditorWidget *parent) :
    QWidget(parent), EditorWidget::Component(parent),
    ionText(parent), currentLine(0)
{
    ionText->addEventListener(QEvent::Resize, this);
    connect(ionText, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(editorUpdateRequest(const QRect &, int)));
    connect(ionText, SIGNAL(blockCountChanged(int)), this, SLOT(editorBlockCountChanged(int)));
    connect(ionText, SIGNAL(cursorPositionChanged()), this, SLOT(editorCursorPositionChanged()));

    editorBlockCountChanged(0);
}


void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), QColor(0xe0, 0xe0, 0xe0));

    QTextBlock block = this->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) this->blockBoundingGeometry(block).translated(this->contentOffset()).top();
    int bottom = top + (int) this->blockBoundingRect(block).height();

    QFont font("Monaco");
    font.setStyleHint(QFont::Courier, QFont::PreferAntialias);
    font.setPixelSize(12);
    painter.setFont(font);
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            if (blockNumber == currentLine) {
                painter.setPen(QColor(0x70, 0x70, 0x70));
            } else {
                painter.setPen(QColor(0xb0, 0xb0, 0xb0));
            }

            painter.drawText(
                        QRectF(
                            3,
                            top,
                            this->width()-5,
                            this->blockBoundingRect(block).height()
                            ),
                        Qt::AlignRight | Qt::AlignVCenter,
                        QString::number(blockNumber + 1)
                        );
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) this->blockBoundingRect(block).height();
        ++blockNumber;
    }
}


int LineNumberArea::getWidth()
{
    int digits = 1;
    int max = qMax(1, ionText->blockCount());
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
        QRect cr = ionText->contentsRect();
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

    if (rect.contains(ionText->viewport()->rect())) {
        ionText->updateViewportMargins();
    }
}

void LineNumberArea::editorBlockCountChanged(int /* newBlockCount */)
{

    //DEBUG_MSG((QString(">> #")+QString("%1").arg(this->getWidth())).toStdString());
    //TODO: performance: check if width changed, and only then set geom and update margins
    ionText->updateViewportMargins();
    QRect cr = ionText->contentsRect();
    setGeometry(QRect(cr.left(), cr.top(), getWidth(), cr.height()));
}

void LineNumberArea::editorCursorPositionChanged()
{
    currentLine = ionText->textCursor().blockNumber();
}

}

