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

class SearchPanel : public QWidget
{
    Q_OBJECT
protected:
    QHBoxLayout *layout;
    QLineEdit *searchText;
    EditorPlugin * editorPlugin;

public:
    explicit SearchPanel(QWidget *parent = 0);

    void paintEvent(QPaintEvent*) {
        QStyleOption styleOption;
        styleOption.init(this);
        QPainter painter(this);
        style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
    }

    void setEditorPlugin(EditorPlugin * editorPlugin) {
        this->editorPlugin = editorPlugin;
    }

    void focusSearchInput() {
        searchText->setFocus();
    }
signals:

public slots:
    void findNext();
    void findPrevious();
};

}
}

#endif // SEARCHPANEL_H
