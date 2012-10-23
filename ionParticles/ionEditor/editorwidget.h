/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONTEXTEDITORWIDGET_H
#define IONTEXTEDITORWIDGET_H

#include <QtCore/QEvent>
#include <QtCore/QMap>
#include <QtGui/QTextBlock>
#include <QtWidgets/QPlainTextEdit>

#include <ionParticles/ionLayout/layoutapi.h>
#include "editorapi.h"

namespace IonEditor {

namespace Private {

class EditorWidget : public Editor
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

    virtual const EditorComponentInfo &getEditorInfo() const {
        return componentInfo;
    }

    int textOptionFlags() { return (int)document()->defaultTextOption().flags(); }
    void setTextOptionFlags(int flags) {
        QTextOption option = document()->defaultTextOption();
        option.setFlags((QTextOption::Flags)flags);
        document()->setDefaultTextOption(option);
    }

    QString getFilePath() { return filePath;}

    float getZoomRatio();
    void setZoomRatio(float ratio);

signals:
    void editorClosing( Editor *editor );
    void editorFocusing( Editor *editor );

public slots:
    virtual void addEventListener(QEvent::Type type, EditorComponent *component) {
        eventListeners[type].append(component);
    }
    void updateViewportMargins();
    void setComponents(QList<EditorComponent*> components) {
        resetComponents();
        this->components = components;
        updateViewportMargins();
    }
    void focusOnLine(int line);
    void saveFile();

protected:
    std::pair<int, int> getSelectedBlockRange();
    void resetComponents();
    void resetHighlighter();
    void addCurrentLineExtraSelection(QList<QTextEdit::ExtraSelection> &extraSelections);

    virtual bool event ( QEvent * event );
    virtual void keyPressEvent ( QKeyEvent * e );
    virtual void keyReleaseEvent ( QKeyEvent * e );
    virtual void closeEvent ( QCloseEvent * event ) {emit(editorClosing(this)); QWidget::closeEvent( event );}
    virtual void focusInEvent ( QFocusEvent * event ) {emit(editorFocusing(this)); QWidget::focusInEvent( event );}
    virtual void wheelEvent(QWheelEvent *e);
    virtual void scrollContentsBy (int dx, int dy) {QPlainTextEdit::scrollContentsBy (  dx,  dy );viewport()->update();}

protected slots:
    void editorCursorPositionChanged();
    void modificationChanged(bool changed);

private:
    QList<EditorComponent* > components;
    QSyntaxHighlighter *highlighter;
    QMap<QEvent::Type, QList<EditorComponent *> > eventListeners;
    QString filePath;
    ComponentInfo componentInfo;
    float zoomRatio;
};

}

}


#endif // IONTEXTEDITORWIDGET_H
