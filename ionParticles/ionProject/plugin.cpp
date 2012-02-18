/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include <ionCore/shared.h>
#include "treeviewpanel.h"
#include "treemodeladapter.h"
#include "treeitem.h"
#include "directorytreesource.h"


#include <QMenuBar>
#include <QFileDialog>

namespace IonProject {
namespace Private {


QSharedPointer<TreeModelSource> Plugin::_defaultTreeModelSourceFactory::operator()(QString dirname) {
    return QSharedPointer<TreeModelSource>(new Private::DirectoryTreeSource(dirname));
}

Plugin::Plugin(QObject *parent) :
    QObject(parent), editorPlugin(NULL), layoutManager(NULL), projectTreeModel(NULL), _treeModelSourceFactory(Plugin::_defaultTreeModelSourceFactory())
{
}

const boost::function<QSharedPointer<TreeModelSource> (QString dirname)> &Plugin::getTreeModelSourceFactory()
{
    return _treeModelSourceFactory;
}

void Plugin::setTreeModelSourceFactory(boost::function<QSharedPointer<TreeModelSource> (QString dirname)> factory)
{
    _treeModelSourceFactory = factory;
}

void Plugin::postLoad()
{
    Q_ASSERT(editorPlugin);
    Q_ASSERT(mainWindow);

    QMenuBar *menuBar = mainWindow->menuBar();
    QMenu *projectMenu = menuBar->addMenu("&Project");
    projectMenu->addAction("&New from source", this, SLOT(onNewProject()));
    projectMenu->addAction("&Update", this, SLOT(onUpdateProject()), QKeySequence(Qt::Key_F5));

    addTreeWidget(getProjectFileTreeModel());
}

void Plugin::onNewProject()
{
    QString dir = QFileDialog::getExistingDirectory(mainWindow, tr("Open Directory"));
    if (dir.length()) {
        getProjectFileTreeModel()->setDirectoryTreeSource(getTreeModelSourceFactory()(dir));
    }
}

void Plugin::onUpdateProject()
{
    getProjectFileTreeModel()->updateFromSource();
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

QSharedPointer<TreeModelAdapter> Plugin::getProjectFileTreeModel()
{
    if (!projectTreeModel) {
        projectTreeModel = QSharedPointer<TreeModelAdapter>(new Private::TreeModelAdapter(getTreeModelSourceFactory()("")));
    }
    return projectTreeModel;
}

void Plugin::addTreeWidget(QSharedPointer<TreeModelAdapter> model)
{
    Private::TreeViewPanel *fileTree = new Private::TreeViewPanel(model);
    layoutManager->add(fileTree);
    connect(fileTree->getTreeView(), SIGNAL(fileActivated(QString, int)), this, SLOT(openFile(QString, int)));
}

void Plugin::addTreeWidget(QSharedPointer<TreeModelSource> modelSource)
{
    QSharedPointer<TreeModelAdapter> model(new Private::TreeModelAdapter(modelSource));
    addTreeWidget(model);
}

QSharedPointer<TreeItemFactory> Plugin::createTreeItemFactory()
{
    return QSharedPointer<TreeItemFactory>(new Private::TreeItemFactoryImpl());
}


}
}


Q_EXPORT_PLUGIN2 ( ionProject, IonProject::Private::Plugin )
