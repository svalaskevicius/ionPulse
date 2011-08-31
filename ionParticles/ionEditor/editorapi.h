#ifndef EDITORAPI_H
#define EDITORAPI_H

#include <QTextBlock>
#include <QEvent>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <ionHeart/layout.h>
#include <ionHeart/plugin.h>

namespace IonEditor {

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
        virtual IonEditor::IEditorComponent *operator()(IEditor *) = 0;
    };
    struct IHighlighter {
        virtual QSyntaxHighlighter *operator()(IEditor *) = 0;
    };
public:
    virtual ~IEditorWidgetFactory(){}
    virtual QSyntaxHighlighter *createHighlighter(IEditor *widget, QString filetype) = 0;
    virtual IonEditor::IEditorComponent *createLineNumberArea(IEditor *widget, QString filetype) = 0;
    virtual IonHeart::IPanelWidget *createEditor(QString path) = 0;

    virtual void registerFileType(QString fileExt, QString fileType) = 0;
    virtual void registerHighlighter(QString const & filetype, IHighlighter *highlighter) = 0;
    virtual void registerLineNumberArea(QString const & filetype, ILineNumberArea *lineNumberArea) = 0;
};

class IEditorPlugin : public IonHeart::IPlugin {
public:
    virtual IEditorWidgetFactory *getEditorWidgetFactory() = 0;
};


}

#endif // EDITORAPI_H
