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
#include "treeitem.h"

#include <QMenuBar>
#include <QFileDialog>

namespace IonProject {

Plugin::Plugin(QObject *parent) :
    QObject(parent), editorPlugin(NULL), layoutManager(NULL), projectTreeModel(NULL)
{
}

void Plugin::postLoad()
{
    Q_ASSERT(editorPlugin);
    Q_ASSERT(mainWindow);

    QMenuBar *menuBar = mainWindow->menuBar();
    QMenu *projectMenu = menuBar->addMenu("&Project");
    projectMenu->addAction("&New from source", this, SLOT(onNewProject()));

    addTreeWidget(getProjectFileTreeModel());
}

void Plugin::onNewProject()
{
    QString dir = QFileDialog::getExistingDirectory(mainWindow, tr("Open Directory"));
    if (dir.length()) {
        Private::DirectoryTreeSource dirTreeSource(dir);
        getProjectFileTreeModel()->setDirectoryTreeSource(dirTreeSource);
    }
}

void Plugin::addParent(BasicPlugin *parent) {
    CHECK_AND_ADD_PARENT(parent, IonEditor::EditorPlugin, editorPlugin = target);
    CHECK_AND_ADD_PARENT(parent, IonLayout::LayoutPlugin, layoutManager = target->getLayoutManager());
}

void Plugin::openFile(QString path, int line)
{
    Q_ASSERT(editorPlugin);
    editorPlugin->openFile(path, line);
}

QSharedPointer<TreeModel> Plugin::getProjectFileTreeModel()
{
    if (!projectTreeModel) {
        Private::DirectoryTreeSource dirTreeSource;
        projectTreeModel = QSharedPointer<TreeModel>(new Private::FileTreeModel(dirTreeSource));
    }
    return projectTreeModel;
}

void Plugin::addTreeWidget(QSharedPointer<TreeModel> model)
{
    Private::TreeWidget *fileTree = new Private::TreeWidget(model);
    layoutManager->add(fileTree);
    connect(fileTree, SIGNAL(fileActivated(QString, int)), this, SLOT(openFile(QString, int)));
}

void Plugin::addTreeWidget(TreeModelSource *modelSource)
{
    QSharedPointer<TreeModel> model(new Private::FileTreeModel(*modelSource));
    addTreeWidget(model);
}

QSharedPointer<TreeItemFactory> Plugin::createTreeItemFactory()
{
    return QSharedPointer<TreeItemFactory>(new Private::TreeItemFactoryImpl());
}


}

Q_EXPORT_PLUGIN2 ( ionProject, IonProject::Plugin )
