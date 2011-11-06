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

namespace IonEditor {

namespace Private {

class EditorWidget;

struct DefaultLineNumberAreaFactory : public LineNumberAreaFactory {
    virtual IonEditor::EditorComponent *operator()(Editor *);
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
    QMap<QString, QSharedPointer<LineNumberAreaFactory> > typeToLineNumberAreaFactoryMap;

    QSyntaxHighlighter *createHighlighter(Editor *widget, QString filetype);
    IonEditor::EditorComponent *createLineNumberArea(Editor *widget, QString filetype);
public:
    EditorWidgetBuilderImpl() {}
    ~EditorWidgetBuilderImpl() {
        typeToHighlighterFactoryMap.clear();
        typeToLineNumberAreaFactoryMap.clear();
    }
    Editor *createEditor(QString path);

    virtual void registerFileType(QString fileExt, QString fileType);
    virtual void registerHighlighterFactory(QString const & filetype, HighlighterFactory *highlighter);
    virtual void registerLineNumberAreaFactory(QString const & filetype, LineNumberAreaFactory *lineNumberArea);
};

}
}

#endif // EDITORWIDGETFACTORY_H
