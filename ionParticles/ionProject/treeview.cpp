/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "treeview.h"
#include "treemodeladapter.h"
#include <QFileInfo>
#include <ionCore/shared.h>

namespace IonProject {
namespace Private {

TreeView::TreeView(QSharedPointer<TreeModelAdapter> dataModel, QWidget *parent) :
    QTreeView(parent), _fiModel(dataModel)
{
    setHeaderHidden(true);

    setIndentation(15);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setModel(_fiModel.data());

    connect(this, SIGNAL(activated( QModelIndex )), this, SLOT(onItemActivated( QModelIndex )));
    connect(this, SIGNAL(collapsed(QModelIndex)), this, SLOT(updateScrollArea(QModelIndex)));
    connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(updateScrollArea(QModelIndex)));
    resizeColumnToContents(0);
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
                emit filterKeyPressed(key);
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
    QTreeView::reset();
    resizeColumnToContents(0);
}


void TreeView::fileEditorFocused(IonEditor::Editor* editor)
{
    DEBUG_MSG("expanding"<<editor->getFilePath());
    expandToPath(editor->getFilePath());
}

void TreeView::expandToPath(QString path)
{
    QModelIndex parent;
    _expandToPath(path, parent);
}

void TreeView::_expandToPath(QString path, const QModelIndex &parent)
{
    int rowCount = _fiModel->rowCount(parent);
    for (int i = 0; i < rowCount; i++) {
        QModelIndex child = _fiModel->index(i, 0, parent);
        QString childPath = _fiModel->getItem(child)->getPath();
        if (path.startsWith(childPath)) {
            expand(child);
            if (path == childPath) {
                if (!_isModelIndexDescendant(child, selectionModel()->currentIndex())) {
                    selectionModel()->clear();
                    selectionModel()->setCurrentIndex(child, QItemSelectionModel::SelectCurrent);
                }
            } else {
                _expandToPath(path, child);
            }
        }
    }
}

bool TreeView::_isModelIndexDescendant(const QModelIndex &parent, const QModelIndex &descendantInQuestion)
{
    QModelIndex current = descendantInQuestion;
    while (current.isValid()) {
        if (current == parent) {
            return true;
        }
        current = current.parent();
    }
    return false;
}

}

}
