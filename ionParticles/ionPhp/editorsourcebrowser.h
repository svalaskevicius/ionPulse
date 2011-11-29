/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef EDITORSOURCEBROWSER_H
#define EDITORSOURCEBROWSER_H

#include <ionHeart/shared.h>
#include <ionParticles/ionEditor/editorapi.h>

namespace IonPhp {

namespace Private {

class EditorSourceBrowser : public IonEditor::EditorComponent
{
public:
    static QString identity() {return "phpEditorSourceBrowser";}

    EditorSourceBrowser(IonEditor::Editor *parent = 0);

    int getWidth() {return 0;}
    void editorEvent(QEvent * event) {
        if (QEvent::KeyRelease == event->type()) {
            QKeyEvent *kev = (QKeyEvent *)event;
            qDebug() << "event: " << kev->key() << kev->modifiers();
        }
    }
};

typedef IonEditor::EditorComponentFactory<EditorSourceBrowser> EditorSourceBrowserFactory;

}

}

#endif // EDITORSOURCEBROWSER_H
