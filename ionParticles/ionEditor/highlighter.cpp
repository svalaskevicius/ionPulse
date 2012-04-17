/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "highlighter.h"
#include <ionCore/shared.h>
#include "editorwidget.h"

namespace IonEditor {
namespace Private {

Highlighter::Highlighter(Editor *parent) :
    QSyntaxHighlighter(parent->document()), ionText(parent)
{

}

void Highlighter::highlightBlock(const QString &)
{
    // generic rule base highlighter
    // default is empty
}


}

}
