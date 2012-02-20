#include "treeviewpanel.h"

#include <QVBoxLayout>

#include "treeviewitemdelegate.h"


namespace IonProject {
namespace Private {


TreeViewPanel::TreeViewPanel(QSharedPointer<TreeModelAdapter> dataModel, QWidget *parent) :
    QWidget(parent), _dataModel(dataModel)
{
    _treeView = new TreeView(dataModel, this);
    Private::TreeViewItemDelegate *viewItemDelegate = new Private::TreeViewItemDelegate(dataModel, _treeView);
    _treeView->setItemDelegate(viewItemDelegate);

    _filterInputField = new QLineEdit(this);
    _filterInputField->hide();

    connect(_filterInputField, SIGNAL(textChanged( QString )), _treeView, SLOT(onFilterTextChanged( QString )));
    connect(_filterInputField, SIGNAL(returnPressed()), this, SLOT(applyFilter( )));
    connect(_treeView, SIGNAL(filterKeyPressed( int )), this, SLOT(filterKey( int )));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);
    layout->addWidget(_treeView);
    layout->addWidget(_filterInputField);
    setLayout(layout);
    setWindowTitle(_dataModel->getTitle());

}

void TreeViewPanel::filterKey(int key)
{
    _filterInputField->setFocus(Qt::ShortcutFocusReason);
    _filterInputField->setText(QString( (char) key - Qt::Key_A + 'a'));
    _filterInputField->show();
}
void TreeViewPanel::applyFilter()
{
    _filterInputField->hide();
    _treeView->setFocus(Qt::ShortcutFocusReason);
}

}
}
