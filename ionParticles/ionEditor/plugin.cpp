#include "plugin.h"

#include <QtPlugin>
#include "editorwidget.h"
#include "editorwidgetfactory.h"
#include <QMessageBox>
#include "editorwidgetfactory.h"
#include <ionHeart/layout.h>
#include <QBoxLayout>
#include "filetreewidget.h"


namespace IonEditor {


Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

Plugin::~Plugin()
{
    // needs to be here as per
    // http://doc.qt.nokia.com/latest/qscopedpointer.html#forward-declared-pointers
}


void Plugin::initialize()
{
    Q_ASSERT(layoutManager);

    IonHeart::ZoneDefinition def;

    def.name = "central";
    def.orientation = Qt::Horizontal;
    def.parentPath = "";
    def.after = "left";
    def.before = "right";
    def.hideIfEmpty = false;
    def.sizeWeight = 700;
    layoutManager->addZone(def);

    def.name = "left";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "";
    def.before = "central";
    def.hideIfEmpty = false;
    def.sizeWeight = 200;
    layoutManager->addZone(def);

    def.name = "leftbottom";
    def.orientation = Qt::Vertical;
    def.parentPath = "left";
    def.after = "left";
    def.before = "";
    def.hideIfEmpty = true;
    def.sizeWeight = 150;
    layoutManager->addZone(def);

    def.name = "right";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "central";
    def.before = "";
    def.hideIfEmpty = true;
    def.sizeWeight = 150;
    layoutManager->addZone(def);

    Private::FileTreeWidget *fileTree = new Private::FileTreeWidget();
    layoutManager->add(fileTree);
    connect(fileTree, SIGNAL(fileActivated(QString)), this, SLOT(openFile(QString)));
}

void Plugin::openFile(QString path)
{
    layoutManager->add(getEditorWidgetBuilder()->createEditor(path));
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
