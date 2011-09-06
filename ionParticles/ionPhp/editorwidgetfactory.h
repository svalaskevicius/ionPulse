#ifndef PHPEDITORWIDGETFACTORY_H
#define PHPEDITORWIDGETFACTORY_H

#include <ionParticles/ionEditor/editorapi.h>


namespace IonPhp {

class EditorWidgetFactory : public IonEditor::EditorWidgetFactory
{
public:
    struct Highlighter : virtual public IonEditor::EditorWidgetFactory::Highlighter {
        virtual QSyntaxHighlighter *operator()(IonEditor::Editor *);
    };
};

}

#endif // PHPEDITORWIDGETFACTORY_H
