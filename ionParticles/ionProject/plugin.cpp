#include "plugin.h"

#include <QtPlugin>
#include <ionHeart/shared.h>
#include <ionParticles/ionEditor/editorapi.h>

namespace IonProject {

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
    }
}

}

Q_EXPORT_PLUGIN2 ( ionProject, IonProject::Plugin )
