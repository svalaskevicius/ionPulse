#include "plugin.h"

#include <QtPlugin>
#include <ionParticles/ionEditor/editorwidget.h>
#include <QMessageBox>
#include "ionHeart/shared.h"

#include <ionParticles/ionEditor/plugin.h>

#include "editorwidgetfactory.h"

namespace IonPhp {

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

void Plugin::initialize()
{
}

void Plugin::addParent(IPlugin *parent) {
    if ("ionEditor" == parent->getName()) {
        IonEditor::IEditorPlugin *editorPlugin = static_cast<IonEditor::IEditorPlugin *>(parent);
        Q_ASSERT(editorPlugin);

        IonEditor::IEditorWidgetFactory *wf = editorPlugin->getEditorWidgetFactory();
        Q_ASSERT(wf);

        wf->registerHighlighter("php", new EditorWidgetFactory::Highlighter());

        wf->registerFileType("php", "php");
        wf->registerFileType("php3", "php");
        wf->registerFileType("phtml", "php");
    }
}

}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Plugin )
