#include "editorwidgetfactory.h"
#include <ionHeart/shared.h>
#include "highlighter.h"

namespace IonPhp {

QSyntaxHighlighter *EditorWidgetBuilder::HighlighterFactory::operator()(IonEditor::Editor *parent)
{
    return new IonPhp::Highlighter(parent->getEditorInstance());
}


}
