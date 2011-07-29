#include "editorwidgetfactory.h"
#include <ionHeart/ionPulse/shared.h>

namespace IonPhp {

IonEditor::Highlighter *EditorWidgetFactory::Highlighter::operator()(IonEditor::EditorWidget *)
{
    DEBUG_MSG("TRALALA");
    return 0;
}


}
