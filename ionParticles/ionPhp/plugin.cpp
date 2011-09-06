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

void Plugin::addParent(BasicPlugin *parent) {
    if (EDITOR_PLUGIN_NAME == parent->getName()) {
        IonEditor::EditorPlugin *editorPlugin = static_cast<IonEditor::EditorPlugin *>(parent);
        Q_ASSERT(editorPlugin);

        IonEditor::EditorWidgetFactory *wf = editorPlugin->getEditorWidgetFactory();
        Q_ASSERT(wf);

        wf->registerHighlighter("php", new EditorWidgetFactory::Highlighter());

        wf->registerFileType("php", "php");
        wf->registerFileType("php3", "php");
        wf->registerFileType("phtml", "php");
    }
}

}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Plugin )
