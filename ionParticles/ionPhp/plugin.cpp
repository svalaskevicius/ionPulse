#include "plugin.h"

#include <QtPlugin>
#include <ionParticles/ionEditor/editorwidget.h>
#include <QMessageBox>
#include "ionHeart/ionPulse/shared.h"

#include <ionParticles/ionEditor/plugin.h>

namespace IonPhp {

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

void Plugin::initialize(IonHeart::MainWindow * /* mainWindow */)
{
}

void Plugin::addParent(IonPlugin *parent) {
    if ("ionEditor" == parent->getName()) {
        IonEditor::Plugin *editorPlugin = dynamic_cast<IonEditor::Plugin *>(parent);
        Q_ASSERT(editorPlugin);
    }
}

}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Plugin )
