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

class EditorWidgetFactoryImpl : public IonEditor::EditorWidgetFactory
{
protected:
    static QMap<QString, QString> fileTypes; // file ending -> file type in factories

    struct DefaultLineNumberArea : public EditorWidgetFactory::LineNumberArea {
        virtual IonEditor::EditorComponent *operator()(Editor *);
    };
    struct DefaultHighlighter : public EditorWidgetFactory::Highlighter {
        virtual QSyntaxHighlighter *operator()(Editor *);
    };
    QString getFileType(QString filePath);
private:
    QMap<QString, QSharedPointer<Highlighter> > m_createHighlighterMap;
    QMap<QString, QSharedPointer<LineNumberArea> > m_createLineNumberAreaMap;

public:
    EditorWidgetFactoryImpl() {}
    ~EditorWidgetFactoryImpl() {
        m_createHighlighterMap.clear();
        m_createLineNumberAreaMap.clear();
    }
    QSyntaxHighlighter *createHighlighter(Editor *widget, QString filetype);
    IonEditor::EditorComponent *createLineNumberArea(Editor *widget, QString filetype);
    IonHeart::PanelWidget *createEditor(QString path);

    virtual void registerFileType(QString fileExt, QString fileType);
    virtual void registerHighlighter(QString const & filetype, Highlighter *highlighter);
    virtual void registerLineNumberArea(QString const & filetype, LineNumberArea *lineNumberArea);
};

}
}

#endif // EDITORWIDGETFACTORY_H
