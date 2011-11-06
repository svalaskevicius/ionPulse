/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include <ionParticles/ionEditor/editorwidget.h>
#include <QMessageBox>
#include "ionHeart/shared.h"

#include "editorwidgetfactory.h"
#include "phptreemodel.h"

namespace IonPhp {

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

void Plugin::postLoad()
{
    PhpTreeSource source(*projectPlugin->getProjectFileTreeModel(), *projectPlugin->createTreeItemFactory());
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

    wf->registerHighlighterFactory("php", new HighlighterFactory());

    wf->registerFileType("php", "php");
    wf->registerFileType("php3", "php");
    wf->registerFileType("phtml", "php");
}

}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Plugin )
