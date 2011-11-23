/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include "editorwidget.h"
#include "editorwidgetfactory.h"
#include <QMessageBox>
#include "editorwidgetfactory.h"
#include <ionParticles/ionLayout/layoutapi.h>
#include <QBoxLayout>
#include "filetreewidget.h"

#include <ionHeart/shared.h>

namespace IonEditor {


Plugin::Plugin(QObject *parent) :
    QObject(parent), layoutManager(NULL), openedFiles()
{
}

Plugin::~Plugin()
{
    // needs to be here as per
    // http://doc.qt.nokia.com/latest/qscopedpointer.html#forward-declared-pointers
}

void Plugin::addParent(BasicPlugin *parent) {
    CHECK_AND_ADD_PARENT(parent, IonLayout::LayoutPlugin, layoutManager = target->getLayoutManager());
}


void Plugin::postLoad()
{
    Q_ASSERT(layoutManager);

    IonLayout::ZoneDefinition def;

    def.name = "central";
    def.orientation = Qt::Horizontal;
    def.parentPath = "";
    def.after = "left";
    def.before = "right";
    def.hideIfEmpty = false;
    def.sizeWeight = 700;
    def.childrenClosable = true;
    layoutManager->addZone(def);

    def.name = "left";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "";
    def.before = "central";
    def.hideIfEmpty = false;
    def.sizeWeight = 200;
    def.childrenClosable = false;
    layoutManager->addZone(def);

    def.name = "leftbottom";
    def.orientation = Qt::Vertical;
    def.parentPath = "left";
    def.after = "left";
    def.before = "";
    def.hideIfEmpty = true;
    def.sizeWeight = 150;
    def.childrenClosable = false;
    layoutManager->addZone(def);

    def.name = "right";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "central";
    def.before = "";
    def.hideIfEmpty = true;
    def.sizeWeight = 150;
    def.childrenClosable = false;
    layoutManager->addZone(def);

    Private::FileTreeWidget *fileTree = new Private::FileTreeWidget();
    layoutManager->add(fileTree);
    connect(fileTree, SIGNAL(fileActivated(QString, int)), this, SLOT(openFile(QString, int)));
}

void Plugin::openFile(QString path, int line)
{
    Editor *widget = NULL;
    QMap<QString, Editor *>::Iterator it = openedFiles.find(path);
    if (openedFiles.end() == it) {
        widget = getEditorWidgetBuilder()->createEditor(path);
        layoutManager->add(widget);
        openedFiles[path] = widget;
    } else {
        widget = it.value();
        layoutManager->focus(widget);
    }
    if (-1 != line) {
        widget->focusOnLine(line);
    }
}


EditorWidgetBuilder *Plugin::getEditorWidgetBuilder()
{
    if (!_editorWidgetFactory) {
        _editorWidgetFactory.reset(new Private::EditorWidgetBuilderImpl());
    }
    return _editorWidgetFactory.data();
}

}

Q_EXPORT_PLUGIN2 ( ionEditor, IonEditor::Plugin )
