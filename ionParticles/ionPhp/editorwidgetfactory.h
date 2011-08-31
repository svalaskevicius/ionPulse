#ifndef PHPEDITORWIDGETFACTORY_H
#define PHPEDITORWIDGETFACTORY_H

#include <ionParticles/ionEditor/editorapi.h>


namespace IonPhp {

class EditorWidgetFactory : public IonEditor::IEditorWidgetFactory
{
public:
    struct Highlighter : virtual public IonEditor::IEditorWidgetFactory::IHighlighter {
        virtual QSyntaxHighlighter *operator()(IonEditor::IEditor *);
    };
};

}

#endif // PHPEDITORWIDGETFACTORY_H
