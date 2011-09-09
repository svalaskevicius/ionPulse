#include "plugin.h"

#include <QtPlugin>
#include <ionHeart/shared.h>
#include "treewidget.h"

namespace IonProject {

Plugin::Plugin(QObject *parent) :
    QObject(parent), editorPlugin(NULL)
{
}

void Plugin::initialize()
{
    Q_ASSERT(editorPlugin);
    Private::TreeWidget *fileTree = new Private::TreeWidget();
    layoutManager->add(fileTree);
    connect(fileTree, SIGNAL(fileActivated(QString)), this, SLOT(openFile(QString)));
}

void Plugin::addParent(BasicPlugin *parent) {
    if (EDITOR_PLUGIN_NAME == parent->getName()) {
        editorPlugin = static_cast<IonEditor::EditorPlugin *>(parent);
    }
}

void Plugin::openFile(QString path)
{
    Q_ASSERT(editorPlugin);
    layoutManager->add(editorPlugin->getEditorWidgetFactory()->createEditor(path));
}

}

Q_EXPORT_PLUGIN2 ( ionProject, IonProject::Plugin )
