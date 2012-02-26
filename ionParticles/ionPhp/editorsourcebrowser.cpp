/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "editorsourcebrowser.h"

namespace IonPhp {

namespace Private {

EditorSourceBrowser::EditorSourceBrowser(IonEditor::Editor *parent, QSharedPointer<StructureStorage> structureStorage)
    : editor(parent)
{
    editor->addEventListener(QEvent::KeyRelease, this);
    editor->addEventListener(QEvent::KeyPress, this);
    this->structureStorage = structureStorage;
}


}
}
