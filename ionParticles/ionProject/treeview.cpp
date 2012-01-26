/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "treeview.h"
#include "treemodeladapter.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <ionHeart/shared.h>

namespace IonProject {
namespace Private {

TreeView::TreeView(QSharedPointer<TreeModel> dataModel, QWidget *parent) :
    QTreeView(parent), _fiModel(dataModel)
{
    setIndentation(15);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setModel(_fiModel.data());

    _filterInputField = new QLineEdit(this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(5, 0, 5, 3);
    layout->addSpacing(100000);
    layout->addWidget(_filterInputField);
    viewport()->setLayout(layout);

    connect(this, SIGNAL(activated( QModelIndex )), this, SLOT(onItemActivated( QModelIndex )));
    connect(this, SIGNAL(collapsed(QModelIndex)), this, SLOT(updateScrollArea(QModelIndex)));
    connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(updateScrollArea(QModelIndex)));
    resizeColumnToContents(0);
    connect(_filterInputField, SIGNAL(textChanged( QString )), this, SLOT(onFilterTextChanged( QString )));
}

TreeView::~TreeView() {
}

void TreeView::onItemActivated(const QModelIndex &index )
{
    if (!index.isValid()) {
        return;
    }
    Q_ASSERT(_fiModel);
    TreeItem* item = _fiModel->getItem(index);
    QFileInfo fi(item->getPath());
    if (fi.isFile()) {
        emit fileActivated(fi.filePath(), item->getLine());
    }
}

void TreeView::keyPressEvent ( QKeyEvent * event ) {
    int key = event->key();
    switch (key) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit activated(currentIndex());
            break;
        default:
            if ((Qt::Key_A <= key) && (Qt::Key_Z >= key)) {
                _filterInputField->setFocus(Qt::ShortcutFocusReason);
                _filterInputField->setText(QString( (char) key - Qt::Key_A + 'a'));
            } else {
                QTreeView::keyPressEvent(event);
            }
    }
}

void TreeView::onFilterTextChanged ( const QString & text ) {
    _fiModel->filter(text);
}

void TreeView::updateScrollArea( const QModelIndex &index )
{
    resizeColumnToContents( index.column() );
}

void TreeView::reset()
{
    resizeColumnToContents(0);
    QTreeView::reset();
}

}

}
