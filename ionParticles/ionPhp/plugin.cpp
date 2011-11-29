/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include <ionParticles/ionEditor/editorwidget.h>
#include "ionHeart/shared.h"

#include "editorwidgetfactory.h"
#include "phptreemodel.h"
#include "editorsourcebrowser.h"

#include <QSqlDatabase>

namespace IonPhp {

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

void Plugin::postLoad()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "phpStructureStorage");
//    db.setDatabaseName("/tmp/db.sqlite");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        throw QString("Unable to establish a database connection.\nPhp plugin requires SQLite support.");
    }
    structureStorage.reset(new StructureStorage("phpStructureStorage"));
    structureStorage->importFileTree(*projectPlugin->getProjectFileTreeModel());

    PhpTreeSource source(*structureStorage, projectPlugin->createTreeItemFactory());
    projectPlugin->addTreeWidget(&source);
}


void Plugin::addParent(BasicPlugin *parent) {
    CHECK_AND_ADD_PARENT(parent, IonEditor::EditorPlugin, addEditorParent(target));
    CHECK_AND_ADD_PARENT(parent, IonProject::ProjectPlugin, projectPlugin = target);
}

void Plugin::addEditorParent(IonEditor::EditorPlugin *editorPlugin)
{
    IonEditor::EditorWidgetBuilder *wf = editorPlugin->getEditorWidgetBuilder();
    Q_ASSERT(wf);

    wf->registerComponentFactory("text/php", new Private::EditorSourceBrowserFactory());
    wf->registerHighlighterFactory("text/php", new HighlighterFactory());

    wf->registerFileType("php", "text/php");
    wf->registerFileType("php3", "text/php");
    wf->registerFileType("phtml", "text/php");
}

}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Plugin )
