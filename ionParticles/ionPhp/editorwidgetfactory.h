#ifndef PHPEDITORWIDGETFACTORY_H
#define PHPEDITORWIDGETFACTORY_H

#include <ionParticles/ionEditor/editorapi.h>


namespace IonPhp {

class EditorWidgetBuilder : public IonEditor::EditorWidgetBuilder
{
public:
    struct HighlighterFactory : virtual public IonEditor::EditorWidgetBuilder::HighlighterFactory {
        virtual QSyntaxHighlighter *operator()(IonEditor::Editor *);
    };
};

}

#endif // PHPEDITORWIDGETFACTORY_H
