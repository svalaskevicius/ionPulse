#include "plugin.h"

#include <QtPlugin>
#include "editorwidget.h"
#include <QMessageBox>
#include "ionPulse/shared.h"

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
        DEBUG_MSG("ohoho");
    }
}

}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Plugin )
