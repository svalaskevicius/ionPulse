/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef EDITORSOURCEBROWSER_H
#define EDITORSOURCEBROWSER_H

#include <ionCore/shared.h>
#include <ionParticles/ionEditor/editorapi.h>
#include "structurestorage.h"

namespace IonPhp {

namespace Private {

class EditorSourceBrowser : public IonEditor::EditorComponent
{
public:
    static QString identity() {return "phpEditorSourceBrowser";}

    EditorSourceBrowser(IonEditor::Editor *parent, QSharedPointer<StructureStorage> structureStorage);

    int getWidth() {return 0;}

    bool editorEvent(QEvent * event)
    {
        switch (event->type()) {
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
                return keyboardHandler(event);
            default: break;
        }
        return false;
    }

    bool keyboardHandler(QEvent * event)
    {
        QKeyEvent *kev = (QKeyEvent *)event;
        switch ( kev->key() ) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if (kev->modifiers().testFlag(Qt::MetaModifier)) {
                    if (QEvent::KeyRelease == event->type()) {
                        goToDefinition(getWordUnderCursor());
                    }
                    return true;
                }
            break;
            case Qt::Key_Backspace:
                if (kev->modifiers().testFlag(Qt::MetaModifier)) {
                    return true;
                }
            break;
        }
        return false;
    }

    QString getWordUnderCursor()
    {
        const QTextCursor &tc = editor->getEditorInstance()->textCursor();
        QString block = tc.block().text();
        int pos = tc.positionInBlock();
        int rpos = pos-1;
        QString ret = "";
        QRegExp allowedCharsLeft("[a-z0-9_:\\.>\\$-]", Qt::CaseInsensitive);
        QRegExp allowedCharsRight("[a-z0-9_]", Qt::CaseInsensitive);
        QChar c = block[pos];
        while (allowedCharsRight.exactMatch(c)) {
            ret += c;
            c = block[++pos];
        }
        c = block[rpos];
        while (allowedCharsLeft.exactMatch(c)) {
            ret = c + ret;
            c = block[--rpos];
        }
        return ret.remove(QRegExp("[>\\.\\(\\):\\$-]$", Qt::CaseInsensitive));
    }

    void goToDefinition(QString word)
    {
        if (!word.length()) {
            return;
        }
        qDebug() << "going to " << word;

        if (!word.contains(QRegExp("[>\\.\\(\\):\\$-]$", Qt::CaseInsensitive))) {
            //structureStorage->findClassByName(word);
        }
    }

protected:
    IonEditor::Editor *editor;
    QSharedPointer<StructureStorage> structureStorage;
};

typedef IonEditor::EditorComponentFactory1<EditorSourceBrowser, QSharedPointer<StructureStorage> > EditorSourceBrowserFactory;

}

}

#endif // EDITORSOURCEBROWSER_H
