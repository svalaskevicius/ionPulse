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
    structureStorage = QSharedPointer<StructureStorage>(NULL);
}

void Plugin::postLoad()
{
    Q_ASSERT(dbXmlPlugin);
    Q_ASSERT(projectPlugin);
    Q_ASSERT(editorPlugin);

    structureStorage = QSharedPointer<StructureStorage>(new StructureStorage(dbXmlPlugin->getStorage()));

    IonEditor::EditorWidgetBuilder *wf = editorPlugin->getEditorWidgetBuilder();
    Q_ASSERT(wf);

    wf->registerComponentFactory("text/php", new Private::EditorSourceBrowserFactory(structureStorage));

    wf->registerFileType("php", "text/php");
    wf->registerFileType("php3", "text/php");
    wf->registerFileType("phtml", "text/php");

    projectPlugin->setTreeModelSourceFactory(
        phpTreeModelSourceDecoratorFactory(
            projectPlugin->getTreeModelSourceFactory(), *structureStorage, projectPlugin->createTreeItemFactory()
        )
    );
}

void Plugin::addParent(BasicPlugin *parent) {
    CHECK_AND_ADD_PARENT(parent, IonEditor::EditorPlugin, editorPlugin = target);
    CHECK_AND_ADD_PARENT(parent, IonProject::ProjectPlugin, projectPlugin = target);
    CHECK_AND_ADD_PARENT(parent, IonDbXml::DbXmlPlugin, dbXmlPlugin = target);
}

}
}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Private::Plugin )
