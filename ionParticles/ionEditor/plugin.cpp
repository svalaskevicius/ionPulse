#include "plugin.h"

#include <QtPlugin>
#include "editorwidget.h"
#include "editorwidgetfactory.h"
#include <QMessageBox>

namespace IonEditor {


Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

void Plugin::initialize(IonHeart::MainWindow *mainWindow)
{
    //QMessageBox(QMessageBox::Information, "hi", "i am a plugin!").exec();
    mainWindow->setCentralWidget(new EditorWidget(new EditorWidgetFactory(), mainWindow));
}

}

Q_EXPORT_PLUGIN2 ( ionEditor, IonEditor::Plugin )
