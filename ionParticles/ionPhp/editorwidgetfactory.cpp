#include "editorwidgetfactory.h"
#include <ionHeart/ionPulse/shared.h>
#include "highlighter.h"

namespace IonPhp {

IonEditor::Highlighter *EditorWidgetFactory::Highlighter::operator()(IonEditor::EditorWidget *parent)
{
    return new IonPhp::Highlighter(parent);
}


}
