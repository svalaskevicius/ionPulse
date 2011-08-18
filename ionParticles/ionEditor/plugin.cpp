#include "plugin.h"

#include <QtPlugin>
#include "editorwidget.h"
#include "editorwidgetfactory.h"
#include <QMessageBox>
#include "editorwidgetfactory.h"
#include <ionHeart/panelwidget.h>
#include <QBoxLayout>



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
}

QList<IonHeart::IPanelWidget *> Plugin::getPanelWidgets()
{
    return QList<IonHeart::IPanelWidget *>() << new EditorWidget(getEditorWidgetFactory());
}

QList<IonHeart::ZoneDefinition> Plugin::getZoneDefinitions()
{
    QList<IonHeart::ZoneDefinition> ret;
    IonHeart::ZoneDefinition def;

    def.name = "central";
    def.orientation = Qt::Horizontal;
    def.parentPath = "";
    def.after = "left";
    def.before = "right";
    ret.append(def);

    def.name = "left";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "";
    def.before = "central";
    ret.append(def);

    def.name = "leftbottom";
    def.orientation = Qt::Vertical;
    def.parentPath = "left";
    def.after = "left";
    def.before = "";
    ret.append(def);

    def.name = "right";
    def.orientation = Qt::Vertical;
    def.parentPath = "";
    def.after = "central";
    def.before = "";
    ret.append(def);

    return ret;
}


EditorWidgetFactory *Plugin::getEditorWidgetFactory()
{
    if (!_editorWidgetFactory) {
        _editorWidgetFactory.reset(new EditorWidgetFactory());
    }
    return _editorWidgetFactory.data();
}

}

Q_EXPORT_PLUGIN2 ( ionEditor, IonEditor::Plugin )
