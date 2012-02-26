#include "searchpanel.h"

namespace IonEditor {

namespace Private {


SearchPanel::SearchPanel(QWidget *parent) : QWidget(parent)
{
    layout = new QHBoxLayout(this);
    searchText = new QLineEdit(this);
    layout->setMargin(0);
    layout->addWidget(searchText);
    layout->addStretch();
    DEBUG_MSG(metaObject()->className());
    //setSizePolicy(searchText->sizePolicy());
    connect(searchText, SIGNAL(returnPressed()), this, SLOT(findNext()));
}

void SearchPanel::findNext() {
    Editor *editor = editorPlugin->getCurrentEditor();
    if (editor) {
        editor->getEditorInstance()->find(searchText->text());
    }
}
void SearchPanel::findPrevious() {
    Editor *editor = editorPlugin->getCurrentEditor();
    if (editor) {
        editor->getEditorInstance()->find(searchText->text(), QTextDocument::FindBackward);
    }
}

}
}
