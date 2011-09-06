#include "editorwidgetfactory.h"
#include <ionHeart/shared.h>
#include "highlighter.h"

namespace IonPhp {

QSyntaxHighlighter *EditorWidgetFactory::Highlighter::operator()(IonEditor::Editor *parent)
{
    return new IonPhp::Highlighter(parent->getEditorInstance());
}


}
