#include "ioneditor.h"

#include <QtPlugin>
#include "iontexteditorwidget.h"
#include <QMessageBox>

IonEditor::IonEditor(QObject *parent) :
    QObject(parent)
{
}

void IonEditor::initialize(IonHeartWindow *mainWindow)
{
    //mainWindow
    QMessageBox(QMessageBox::Information, "hi", "i am a plugin!").exec();
}

Q_EXPORT_PLUGIN2 ( ionEditor, IonEditor )
