#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include "linenumberarea.h"
#include "highlighter.h"
#include <QSharedPointer>
#include <ionHeart/layout.h>
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
    IonHeart::PanelWidget *createEditor(QString path);

    virtual void registerFileType(QString fileExt, QString fileType);
    virtual void registerHighlighterFactory(QString const & filetype, HighlighterFactory *highlighter);
    virtual void registerLineNumberAreaFactory(QString const & filetype, LineNumberAreaFactory *lineNumberArea);
};

}
}

#endif // EDITORWIDGETFACTORY_H
