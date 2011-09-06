#ifndef IONTEXTEDITORWIDGET_H
#define IONTEXTEDITORWIDGET_H

#include <QPlainTextEdit>
#include <QEvent>
#include <QMap>
#include <QTextBlock>
#include <ionHeart/layout.h>
#include "editorapi.h"


namespace IonEditor {

namespace Private {

class EditorWidget : public QPlainTextEdit, public Editor, public IonHeart::PanelWidget
{
    Q_OBJECT
public:
    /**
     * a class for defining separate components, used to
     * open some of the protected functionality
     */
    class ComponentInfo : public EditorComponentInfo {
    public:
        EditorWidget *widget;
        ComponentInfo(EditorWidget *widget): widget(widget) {}
        virtual ~ComponentInfo() {}
        inline QTextBlock firstVisibleBlock() const
        {
            return widget->firstVisibleBlock();
        }
        inline QRectF blockBoundingGeometry(const QTextBlock &block) const
        {
            return widget->blockBoundingGeometry(block);
        }
        inline QPointF contentOffset() const
        {
            return widget->contentOffset();
        }
        inline QRectF blockBoundingRect(const QTextBlock &block) const
        {
            return widget->blockBoundingRect(block);
        }
    };
    explicit EditorWidget(QString filePath);
    virtual ~EditorWidget();
    void setHighlighter(QSyntaxHighlighter *highlighter) {
        resetHighlighter();
        this->highlighter = highlighter;
    }
protected:
    bool event ( QEvent * event );
    void resetComponents();
    void resetHighlighter();

public:
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelTitle();
    virtual QString getPanelZone() {return "central";}

public:
    virtual const EditorComponentInfo &getEditorInfo() const {
        return componentInfo;
    }
    virtual void addEventListener(QEvent::Type type, EditorComponent *component) {
        eventListeners[type].append(component);
    }
    void updateViewportMargins();
    void setComponents(QList<EditorComponent*> components) {
        resetComponents();
        this->components = components;
        updateViewportMargins();
    }
    virtual QPlainTextEdit* getEditorInstance() {return this;}


protected slots:
    void editorCursorPositionChanged();
protected:
    void addCurrentLineExtraSelection(QList<QTextEdit::ExtraSelection> &extraSelections);

private:
    QList<EditorComponent* > components;
    QSyntaxHighlighter *highlighter;
    QMap<QEvent::Type, QList<EditorComponent *> > eventListeners;
    QString filePath;
    ComponentInfo componentInfo;
signals:

public slots:
public:
};

}

}


#endif // IONTEXTEDITORWIDGET_H
