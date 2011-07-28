#ifndef IONTEXTEDITORWIDGET_H
#define IONTEXTEDITORWIDGET_H

#include <QPlainTextEdit>
#include <QEvent>
#include <QMap>
#include <QTextBlock>

namespace IonEditor {

class EditorWidgetFactory;
class LineNumberArea;
class Highlighter;


class EditorWidget : public QPlainTextEdit
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
    explicit EditorWidget(EditorWidgetFactory *widgetFactory, QWidget *parent = 0);
    virtual ~EditorWidget();
    void addEventListener(QEvent::Type type, Component *component) {
        eventListeners[type].append(component);
    }
    void updateViewportMargins();
protected:
    EditorWidgetFactory *widgetFactory;
    bool event ( QEvent * event );
    void addComponent(Component *component) {
        components.append(component);
    }

private slots:

private:
    LineNumberArea *lineNumberArea;
    Highlighter *highlighter;
    QList<Component *> components;
    QMap<QEvent::Type, QList<Component *> > eventListeners;

signals:

public slots:
};


}


#endif // IONTEXTEDITORWIDGET_H
