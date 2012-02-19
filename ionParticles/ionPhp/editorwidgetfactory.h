/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PHPEDITORWIDGETFACTORY_H
#define PHPEDITORWIDGETFACTORY_H

#include <ionParticles/ionEditor/editorapi.h>


namespace IonPhp {
namespace Private {

struct HighlighterFactory : virtual public IonEditor::HighlighterFactory {
    virtual QSyntaxHighlighter *operator()(IonEditor::Editor *);
};

}
}

#endif // PHPEDITORWIDGETFACTORY_H
