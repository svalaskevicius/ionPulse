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

EditorWidgetFactory *Plugin::getEditorWidgetFactory()
{
    if (!_editorWidgetFactory) {
        _editorWidgetFactory.reset(new EditorWidgetFactory());
    }
    return _editorWidgetFactory.data();
}

}

Q_EXPORT_PLUGIN2 ( ionEditor, IonEditor::Plugin )
