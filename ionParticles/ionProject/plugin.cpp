/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include <ionHeart/shared.h>
#include "treewidget.h"
#include "filetreemodel.h"

namespace IonProject {

Plugin::Plugin(QObject *parent) :
    QObject(parent), editorPlugin(NULL), layoutManager(NULL), projectTreeModel(NULL)
{
}

void Plugin::postLoad()
{
    Q_ASSERT(editorPlugin);

    Private::TreeWidget *fileTree = new Private::TreeWidget(getProjectFileTreeModel());

    layoutManager->add(fileTree);
    connect(fileTree, SIGNAL(fileActivated(QString)), this, SLOT(openFile(QString)));
}

void Plugin::addParent(BasicPlugin *parent) {
    CHECK_AND_ADD_PARENT(parent, IonEditor::EditorPlugin, editorPlugin = target);
    CHECK_AND_ADD_PARENT(parent, IonLayout::LayoutPlugin, layoutManager = target->getLayoutManager());
}

void Plugin::openFile(QString path)
{
    Q_ASSERT(editorPlugin);
    layoutManager->add(editorPlugin->getEditorWidgetBuilder()->createEditor(path));
}

TreeModel *Plugin::getProjectFileTreeModel()
{
    if (!projectTreeModel) {
        Private::DirectoryTreeSource dirTreeSource("/Users/svalaskevicius/csDisk/warner.development.local");
        projectTreeModel = new Private::FileTreeModel(&dirTreeSource);
    }
    return projectTreeModel;
}

}

Q_EXPORT_PLUGIN2 ( ionProject, IonProject::Plugin )
