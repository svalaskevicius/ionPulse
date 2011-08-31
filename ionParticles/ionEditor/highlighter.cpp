#include "highlighter.h"
#include <ionHeart/shared.h>
#include "editorwidget.h"

namespace IonEditor {
namespace Private {

Highlighter::Highlighter(QPlainTextEdit *parent) :
    QSyntaxHighlighter(parent->document()), ionText(parent)
{

}

void Highlighter::highlightBlock(const QString &)
{
    printf("GEN\n");
    // generic rule base highlighter // default is empty
}


}

}
