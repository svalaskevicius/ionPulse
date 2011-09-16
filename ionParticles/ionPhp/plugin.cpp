/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include <ionParticles/ionEditor/editorwidget.h>
#include <QMessageBox>
#include "ionHeart/shared.h"

#include "editorwidgetfactory.h"

namespace IonPhp {

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
}

void Plugin::initialize()
{
}

void Plugin::addParent(BasicPlugin *parent) {
    if (EDITOR_PLUGIN_NAME == parent->getName()) {
        IonEditor::EditorPlugin *editorPlugin = static_cast<IonEditor::EditorPlugin *>(parent);
        Q_ASSERT(editorPlugin);

        IonEditor::EditorWidgetBuilder *wf = editorPlugin->getEditorWidgetBuilder();
        Q_ASSERT(wf);

        wf->registerHighlighterFactory("php", new HighlighterFactory());

        wf->registerFileType("php", "php");
        wf->registerFileType("php3", "php");
        wf->registerFileType("phtml", "php");
    }
}

}

Q_EXPORT_PLUGIN2 ( ionPhp, IonPhp::Plugin )
