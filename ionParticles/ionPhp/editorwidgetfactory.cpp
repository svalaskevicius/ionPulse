/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "editorwidgetfactory.h"
#include <ionCore/shared.h>
#include "highlighter.h"

namespace IonPhp {
namespace Private {

QSyntaxHighlighter *HighlighterFactory::operator()(IonEditor::Editor *parent, QString filetype)
{
    return new IonPhp::Private::Highlighter(parent);
}

}
}
