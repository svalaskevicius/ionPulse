#ifndef EDITORAPI_H
#define EDITORAPI_H

#include <QTextBlock>
#include <QEvent>
#include <QPlainTextEdit>

namespace IonEditor {

class Highlighter;
class LineNumberArea;
class EditorWidget;

class IEditorComponentInfo {
public:
    virtual ~IEditorComponentInfo() {}
    virtual QTextBlock firstVisibleBlock() const = 0;
    virtual QRectF blockBoundingGeometry(const QTextBlock &block) const = 0;
    virtual QPointF contentOffset() const = 0;
    virtual QRectF blockBoundingRect(const QTextBlock &block) const = 0;
};

class IEditorComponent {
public:
    virtual ~IEditorComponent() {}
    virtual void editorEvent(QEvent * ) = 0;
    virtual int getWidth() = 0;
};

class IEditor {
public:
    virtual ~IEditor() {}
    virtual const IEditorComponentInfo &getEditorInfo() const = 0;
    virtual void addEventListener(QEvent::Type type, IEditorComponent *component) = 0;
    virtual void updateViewportMargins() = 0;
    virtual void setComponents(QList<IEditorComponent* > components) = 0;
    virtual QPlainTextEdit* getEditorInstance() = 0;
};

class IEditorWidgetFactory
{
protected:
    struct ILineNumberArea {
        virtual IonEditor::LineNumberArea *operator()(EditorWidget *) = 0;
    };
    struct IHighlighter {
        virtual IonEditor::Highlighter *operator()(EditorWidget *) = 0;
    };
public:
    virtual ~IEditorWidgetFactory(){}
    virtual IonEditor::Highlighter *createHighlighter(EditorWidget *widget, QString filetype) = 0;
    virtual IonEditor::LineNumberArea *createLineNumberArea(EditorWidget *widget, QString filetype) = 0;
    virtual IonHeart::IPanelWidget *createEditor(QString path) = 0;

    virtual void registerHighlighter(QString filetype, IHighlighter *highlighter) = 0;
    virtual void registerLineNumberArea(QString filetype, ILineNumberArea *lineNumberArea) = 0;
};

class IEditorPlugin {
    virtual IEditorWidgetFactory *getEditorWidgetFactory() = 0;
};


}

#endif // EDITORAPI_H
