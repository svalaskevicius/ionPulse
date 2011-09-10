#ifndef PHPEDITORWIDGETFACTORY_H
#define PHPEDITORWIDGETFACTORY_H

#include <ionParticles/ionEditor/editorapi.h>


namespace IonPhp {

struct HighlighterFactory : virtual public IonEditor::HighlighterFactory {
    virtual QSyntaxHighlighter *operator()(IonEditor::Editor *);
};

}

#endif // PHPEDITORWIDGETFACTORY_H
