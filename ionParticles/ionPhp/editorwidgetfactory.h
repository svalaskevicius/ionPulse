#ifndef PHPEDITORWIDGETFACTORY_H
#define PHPEDITORWIDGETFACTORY_H

#include <ionParticles/ionEditor/editorwidgetfactory.h>


namespace IonPhp {
class IonEditor::Highlighter;
struct EditorWidgetFactory : public IonEditor::EditorWidgetFactory
{
public:
    struct Highlighter : virtual public IonEditor::EditorWidgetFactory::Highlighter {
        virtual IonEditor::Highlighter *operator()(IonEditor::EditorWidget *);
    };
};

}

#endif // PHPEDITORWIDGETFACTORY_H
