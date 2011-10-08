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
#include "treemodel.h"

namespace IonProject {

Plugin::Plugin(QObject *parent) :
    QObject(parent), editorPlugin(NULL), layoutManager(NULL)
{
}

void Plugin::postLoad()
{
    Q_ASSERT(editorPlugin);

    Private::DirectoryTreeSource dirTreeSource("/Users/svalaskevicius/csDisk/warner.development.local");
    Private::TreeWidget *fileTree = new Private::TreeWidget(new Private::TreeModel(&dirTreeSource));

    layoutManager->add(fileTree);
    connect(fileTree, SIGNAL(fileActivated(QString)), this, SLOT(openFile(QString)));
}

void Plugin::addParent(BasicPlugin *parent) {
    if (EDITOR_PLUGIN_NAME == parent->getName()) {
        editorPlugin = static_cast<IonEditor::EditorPlugin *>(parent);
    } else if (LAYOUT_PLUGIN_NAME == parent->getName()) {
        IonLayout::LayoutPlugin *layoutPlugin = static_cast<IonLayout::LayoutPlugin *>(parent);
        Q_ASSERT(layoutPlugin);
        layoutManager = layoutPlugin->getLayoutManager();
    }

}

void Plugin::openFile(QString path)
{
    Q_ASSERT(editorPlugin);
    layoutManager->add(editorPlugin->getEditorWidgetBuilder()->createEditor(path));
}

}

Q_EXPORT_PLUGIN2 ( ionProject, IonProject::Plugin )
