#include "linenumberarea.h"
#include <QPainter>
#include <QTextBlock>
#include <QPaintEvent>
#include <ionPulse/shared.h>

namespace IonEditor {

LineNumberArea::LineNumberArea(EditorWidget *parent) :
    QWidget(parent), EditorWidget::Component(parent),
    ionText(parent)
{
    ionText->addEventListener(QEvent::Resize, this);
    connect(ionText, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(editorUpdateRequest(const QRect &, int)));
    connect(ionText, SIGNAL(blockCountChanged(int)), this, SLOT(editorBlockCountChanged(int)));
    editorBlockCountChanged(0);
}


void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = this->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) this->blockBoundingGeometry(block).translated(this->contentOffset()).top();
    int bottom = top + (int) this->blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, this->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
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

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void LineNumberArea::editorEvent(QEvent * event)
{
    DEBUG_MSG((QString(">> ")+QString("%1").arg(event->type())).toStdString());
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
    if (dy)
        scroll(0, dy);
    else
        update(0, rect.y(), width(), rect.height());

    if (rect.contains(ionText->viewport()->rect()))
        ionText->updateViewportMargins();
}

void LineNumberArea::editorBlockCountChanged(int /* newBlockCount */)
{

    DEBUG_MSG((QString(">> #")+QString("%1").arg(this->getWidth())).toStdString());
    ionText->updateViewportMargins();
    QRect cr = ionText->contentsRect();
    setGeometry(QRect(cr.left(), cr.top(), getWidth(), cr.height()));
}

}

