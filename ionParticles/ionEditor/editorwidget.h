/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONTEXTEDITORWIDGET_H
#define IONTEXTEDITORWIDGET_H

#include <QPlainTextEdit>
#include <QEvent>
#include <QMap>
#include <QTextBlock>
#include <ionParticles/ionLayout/layoutapi.h>
#include "editorapi.h"

namespace IonEditor {

namespace Private {

class EditorWidget : public QPlainTextEdit, public Editor
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
    void focusOnLine(int line);
protected:
    std::pair<int, int> getSelectedBlockRange();
    bool event ( QEvent * event );
    void resetComponents();
    void resetHighlighter();
    void keyPressEvent ( QKeyEvent * e );
    void keyReleaseEvent ( QKeyEvent * e );

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
    void modificationChanged(bool changed);
protected:
    void addCurrentLineExtraSelection(QList<QTextEdit::ExtraSelection> &extraSelections);
    void closeEvent ( QCloseEvent * event ) {emit(editorClosing(this)); QWidget::closeEvent( event );}
    void focusInEvent ( QFocusEvent * event ) {emit(editorFocusing(this)); QWidget::focusInEvent( event );}
    virtual void scrollContentsBy (int dx, int dy) {QPlainTextEdit::scrollContentsBy (  dx,  dy );viewport()->update();}
private:
    QList<EditorComponent* > components;
    QSyntaxHighlighter *highlighter;
    QMap<QEvent::Type, QList<EditorComponent *> > eventListeners;
    QString filePath;
    ComponentInfo componentInfo;
signals:
    void editorClosing( Editor *editor );
    void editorFocusing( Editor *editor );
    void updatePanelTitle( IonLayout::PanelWidget *panel );
public slots:
public:
    virtual void saveFile();
};

}

}


#endif // IONTEXTEDITORWIDGET_H
