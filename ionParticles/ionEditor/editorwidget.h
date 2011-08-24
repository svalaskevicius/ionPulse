#ifndef IONTEXTEDITORWIDGET_H
#define IONTEXTEDITORWIDGET_H

#include <QPlainTextEdit>
#include <QEvent>
#include <QMap>
#include <QTextBlock>
#include <ionHeart/layout.h>

namespace IonEditor {

class EditorWidgetFactory;
class LineNumberArea;
class Highlighter;


class EditorWidget : public QPlainTextEdit, public IonHeart::IPanelWidget
{
    Q_OBJECT
public:
    /**
     * a class for defining separate components, used to
     * open some of the protected functionality
     */
    class Component {
    public:
        EditorWidget *widget;
        Component(EditorWidget *widget): widget(widget) {}
        virtual ~Component() {}
        virtual void editorEvent(QEvent * ) = 0;
        virtual int getWidth() {return 0;}
    protected:
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
    explicit EditorWidget();
    virtual ~EditorWidget();
    void addEventListener(QEvent::Type type, Component *component) {
        eventListeners[type].append(component);
    }
    void updateViewportMargins();
    void setComponents(QList<Component *> components) {
        resetComponents();
        this->components = components;
        updateViewportMargins();
    }
    void setHighlighter(Highlighter *highlighter) {
        resetHighlighter();
        this->highlighter = highlighter;
    }
protected:
    bool event ( QEvent * event );
    void resetComponents();
    void resetHighlighter();

public:
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelTitle() {return "title";}
    virtual QString getPanelZone() {return "central";}

private slots:

private:
    QList<Component *> components;
    Highlighter *highlighter;
    QMap<QEvent::Type, QList<Component *> > eventListeners;

signals:

public slots:
public:
    static QMap<QString, QString> fileTypes; // file ending -> file type in factories
};


}


#endif // IONTEXTEDITORWIDGET_H
