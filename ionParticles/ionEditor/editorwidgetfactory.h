/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include "linenumberarea.h"
#include "highlighter.h"
#include <QSharedPointer>
#include <ionParticles/ionLayout/layoutapi.h>
#include "editorapi.h"
#include <QMultiMap>

namespace IonEditor {

namespace Private {

class EditorWidget;

struct DefaultLineNumberAreaFactory : public EditorComponentFactory {
    virtual IonEditor::EditorComponent *operator()(Editor *);
    virtual QString getIdentifier() {return "LineNumberArea";}
};
struct DefaultHighlighterFactory : public HighlighterFactory {
    virtual QSyntaxHighlighter *operator()(Editor *);
};

class EditorWidgetBuilderImpl : public IonEditor::EditorWidgetBuilder
{
protected:
    QString getFileType(QString filePath);
private:
    static QMap<QString, QString> fileExtToTypeMap;

    QMap<QString, QSharedPointer<HighlighterFactory> > typeToHighlighterFactoryMap;
    QMultiMap<QString, QSharedPointer<EditorComponentFactory> > typeToComponentFactoryMap;

    QSyntaxHighlighter *createHighlighter(Editor *widget, QString filetype);
public:
    EditorWidgetBuilderImpl() {}
    ~EditorWidgetBuilderImpl() {
        typeToHighlighterFactoryMap.clear();
        typeToComponentFactoryMap.clear();
    }
    Editor *createEditor(QString path);

    virtual void registerFileType(QString fileExt, QString fileType);
    virtual void registerHighlighterFactory(QString const & filetype, HighlighterFactory *highlighter);
    virtual void registerComponentFactory(QString const & filetype, EditorComponentFactory *component);
};

}
}

#endif // EDITORWIDGETFACTORY_H
