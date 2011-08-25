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
        IonEditor::Plugin *editorPlugin = dynamic_cast<IonEditor::Plugin *>(parent);
        Q_ASSERT(editorPlugin);
        editorPlugin->getEditorWidgetFactory()->registerHighlighter("php", new EditorWidgetFactory::Highlighter());
        IonEditor::EditorWidgetFactory::fileTypes.insert("php",   "php");
        IonEditor::EditorWidgetFactory::fileTypes.insert("php3",  "php");
        IonEditor::EditorWidgetFactory::fileTypes.insert("phtml", "php");
    }
}

}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Plugin )
