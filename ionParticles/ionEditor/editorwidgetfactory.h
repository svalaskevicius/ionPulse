#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include "linenumberarea.h"
#include "highlighter.h"
#include <QSharedPointer>
#include <ionHeart/layout.h>
#include "editorapi.h"

namespace IonEditor {

class EditorWidget;

class EditorWidgetFactory : public IEditorWidgetFactory
{
protected:

    struct LineNumberArea : public IEditorWidgetFactory::ILineNumberArea {
        virtual IonEditor::LineNumberArea *operator()(EditorWidget *);
    };
    struct Highlighter : public IEditorWidgetFactory::IHighlighter {
        virtual IonEditor::Highlighter *operator()(EditorWidget *);
    };
    QString getFileType(QString filePath);
private:
    QMap<QString, QSharedPointer<IHighlighter> > m_createHighlighterMap;
    QMap<QString, QSharedPointer<ILineNumberArea> > m_createLineNumberAreaMap;

public:
    static QMap<QString, QString> fileTypes; // file ending -> file type in factories

    EditorWidgetFactory() {}
    IonEditor::Highlighter *createHighlighter(EditorWidget *widget, QString filetype);
    IonEditor::LineNumberArea *createLineNumberArea(EditorWidget *widget, QString filetype);
    IonHeart::IPanelWidget *createEditor(QString path);

    virtual void registerHighlighter(QString filetype, IHighlighter *highlighter);
    virtual void registerLineNumberArea(QString filetype, ILineNumberArea *lineNumberArea);
};

}

#endif // EDITORWIDGETFACTORY_H
