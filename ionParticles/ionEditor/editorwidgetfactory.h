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

class EditorWidgetFactory : public IEditorWidgetFactory
{
protected:
    static QMap<QString, QString> fileTypes; // file ending -> file type in factories

    struct LineNumberArea : public IEditorWidgetFactory::ILineNumberArea {
        virtual IonEditor::IEditorComponent *operator()(IEditor *);
    };
    struct Highlighter : public IEditorWidgetFactory::IHighlighter {
        virtual QSyntaxHighlighter *operator()(IEditor *);
    };
    QString getFileType(QString filePath);
private:
    QMap<QString, QSharedPointer<IHighlighter> > m_createHighlighterMap;
    QMap<QString, QSharedPointer<ILineNumberArea> > m_createLineNumberAreaMap;

public:
    EditorWidgetFactory() {}
    ~EditorWidgetFactory() {
        m_createHighlighterMap.clear();
        m_createLineNumberAreaMap.clear();
    }
    QSyntaxHighlighter *createHighlighter(IEditor *widget, QString filetype);
    IonEditor::IEditorComponent *createLineNumberArea(IEditor *widget, QString filetype);
    IonHeart::IPanelWidget *createEditor(QString path);

    virtual void registerFileType(QString fileExt, QString fileType);
    virtual void registerHighlighter(QString const & filetype, IHighlighter *highlighter);
    virtual void registerLineNumberArea(QString const & filetype, ILineNumberArea *lineNumberArea);
};

}
}

#endif // EDITORWIDGETFACTORY_H
