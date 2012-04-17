/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include <ionParticles/ionEditor/editorwidget.h>
#include "ionCore/shared.h"

#include "phptreemodelsource.h"
#include "editorsourcebrowser.h"

#include <QSqlDatabase>

namespace IonPhp {
namespace Private {

QSharedPointer<IonProject::TreeModelSource> Plugin::phpTreeModelSourceDecoratorFactory::operator()(QString dirname)
{
    return QSharedPointer<IonProject::TreeModelSource>(new Private::PhpTreeModelSourceDecorator(origFactory(dirname), structureStorage, treeItemFactory));
}

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

void Plugin::preLoad()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "phpStructureStorage");
//    db.setDatabaseName("/tmp/db.sqlite");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        throw QString("Unable to establish a database connection.\nPhp plugin requires SQLite support.");
    }
    structureStorage = QSharedPointer<StructureStorage>(new StructureStorage("phpStructureStorage"));
}

void Plugin::postLoad()
{
}

void Plugin::addParent(BasicPlugin *parent) {
    CHECK_AND_ADD_PARENT(parent, IonEditor::EditorPlugin, addEditorParent(target));
    CHECK_AND_ADD_PARENT(parent, IonProject::ProjectPlugin, addProjectParent(target));
}

void Plugin::addEditorParent(IonEditor::EditorPlugin *editorPlugin)
{
    IonEditor::EditorWidgetBuilder *wf = editorPlugin->getEditorWidgetBuilder();
    Q_ASSERT(wf);

    wf->registerComponentFactory("text/php", new Private::EditorSourceBrowserFactory(structureStorage));

    wf->registerFileType("php", "text/php");
    wf->registerFileType("php3", "text/php");
    wf->registerFileType("phtml", "text/php");
}

void Plugin::addProjectParent(IonProject::ProjectPlugin *projectPlugin)
{
    projectPlugin->setTreeModelSourceFactory(
        phpTreeModelSourceDecoratorFactory(
            projectPlugin->getTreeModelSourceFactory(), *structureStorage, projectPlugin->createTreeItemFactory()
        )
    );
}

}
}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Private::Plugin )
