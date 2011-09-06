#ifndef EDITORAPI_H
#define EDITORAPI_H

#include <QTextBlock>
#include <QEvent>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <ionHeart/layout.h>
#include <ionHeart/plugin.h>

#define EDITOR_PLUGIN_NAME "ionEditor"

namespace IonEditor {

class EditorComponentInfo {
public:
    virtual ~EditorComponentInfo() {}
    virtual QTextBlock firstVisibleBlock() const = 0;
    virtual QRectF blockBoundingGeometry(const QTextBlock &block) const = 0;
    virtual QPointF contentOffset() const = 0;
    virtual QRectF blockBoundingRect(const QTextBlock &block) const = 0;
};

class EditorComponent {
public:
    virtual ~EditorComponent() {}
    virtual void editorEvent(QEvent * ) = 0;
    virtual int getWidth() = 0;
};

class Editor {
public:
    virtual ~Editor() {}
    virtual const EditorComponentInfo &getEditorInfo() const = 0;
    virtual void addEventListener(QEvent::Type type, EditorComponent *component) = 0;
    virtual void updateViewportMargins() = 0;
    virtual void setComponents(QList<EditorComponent* > components) = 0;
    virtual QPlainTextEdit* getEditorInstance() = 0;
};

class EditorWidgetFactory
{
protected:
    struct LineNumberArea {
        virtual IonEditor::EditorComponent *operator()(Editor *) = 0;
    };
    struct Highlighter {
        virtual QSyntaxHighlighter *operator()(Editor *) = 0;
    };
public:
    virtual ~EditorWidgetFactory(){}
    virtual QSyntaxHighlighter *createHighlighter(Editor *widget, QString filetype) = 0;
    virtual IonEditor::EditorComponent *createLineNumberArea(Editor *widget, QString filetype) = 0;
    virtual IonHeart::PanelWidget *createEditor(QString path) = 0;

    virtual void registerFileType(QString fileExt, QString fileType) = 0;
    virtual void registerHighlighter(QString const & filetype, Highlighter *highlighter) = 0;
    virtual void registerLineNumberArea(QString const & filetype, LineNumberArea *lineNumberArea) = 0;
};

class EditorPlugin : public IonHeart::BasicPlugin {
public:
    virtual EditorWidgetFactory *getEditorWidgetFactory() = 0;
};


}

#endif // EDITORAPI_H
