#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QLineEdit>
#include <QHBoxLayout>

#include <ionParticles/ionLayout/layoutapi.h>
#include <ionCore/shared.h>

#include "editorapi.h"

namespace IonEditor {

namespace Private {

class SearchPanel : public QWidget, public IonLayout::PanelWidget
{
    Q_OBJECT
protected:
    QHBoxLayout *layout;
    QLineEdit *searchText;
    std::function<Editor *()> __getActiveEditor;

    inline Editor *getActiveEditor() {
        if (!__getActiveEditor) {
            throw std::runtime_error("active editor getter not set for searchPanel");
        }
        return __getActiveEditor();
    }
public:
    explicit SearchPanel(QWidget *parent = 0);
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelZone() {return "central/central_footer";}

    void paintEvent(QPaintEvent*) {
        QStyleOption styleOption;
        styleOption.init(this);
        QPainter painter(this);
        style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
    }

    void setActiveEditorGetter(std::function<Editor *()> getter) {
        __getActiveEditor = getter;
    }

    void focusSearchInput() {
        searchText->setFocus();
    }
signals:

public slots:
    void findNext() {
        Editor *editor = getActiveEditor();
        if (editor) {
            editor->getEditorInstance()->find(searchText->text());
        }
    }
    void findPrevious() {
        Editor *editor = getActiveEditor();
        if (editor) {
            editor->getEditorInstance()->find(searchText->text(), QTextDocument::FindBackward);
        }
    }
};

}
}

#endif // SEARCHPANEL_H
