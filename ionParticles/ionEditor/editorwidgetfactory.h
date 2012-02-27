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

typedef EditorComponentFactory<IonEditor::Private::LineNumberArea> DefaultLineNumberAreaFactory;

struct DefaultHighlighterFactory : public HighlighterFactory {
    virtual QSyntaxHighlighter *operator()(Editor *, QString);
};

class EditorWidgetBuilderImpl : public IonEditor::EditorWidgetBuilder
{
protected:
    QString getFileType(QString filePath);
private:
    static QMap<QString, QString> fileExtToTypeMap;

    QMap<QString, QSharedPointer<HighlighterFactory> > typeToHighlighterFactoryMap;
    QMultiMap<QString, QSharedPointer<EditorComponentFactoryBase> > typeToComponentFactoryMap;

    QSyntaxHighlighter *createHighlighter(Editor *widget, QString filetype);
    QList<EditorComponent*> createComponents(Editor *widget, QString fileType);
public:
    EditorWidgetBuilderImpl() {}
    ~EditorWidgetBuilderImpl() {
        typeToHighlighterFactoryMap.clear();
        typeToComponentFactoryMap.clear();
    }
    Editor *createEditor(QString path);

    virtual void registerFileType(QString fileExt, QString fileType);
    virtual void registerHighlighterFactory(QString const & filetype, HighlighterFactory *highlighter);
    virtual void registerComponentFactory(QString const & filetype, EditorComponentFactoryBase *component);
protected:
};

}
}

#endif // EDITORWIDGETFACTORY_H
