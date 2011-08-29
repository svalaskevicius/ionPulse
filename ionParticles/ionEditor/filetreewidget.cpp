#include "filetreewidget.h"

namespace IonEditor {


FileTreeWidget::FileTreeWidget(QWidget *parent) :
    QTreeView(parent), _fiModel(NULL)
{
    // todo: add filtering, and move qtreeview to subelement
    _fiModel = new QFileSystemModel(this);
   // _fiModel->setNameFilters(QStringList() << "*.php");
    _fiModel->setNameFilterDisables(false);
    _fiModel->setRootPath(QDir::homePath());
    this->setModel(_fiModel);
    this->setRootIndex(_fiModel->index(QDir::homePath()));
    for (int i=1;i<_fiModel->columnCount();i++) {
        this->setColumnHidden(i, true);
    }
    connect(this, SIGNAL(activated( QModelIndex )), this, SLOT(onItemActivated( QModelIndex )));
}

void FileTreeWidget::onItemActivated(const QModelIndex &index )
{
    if (!index.isValid()) {
        return;
    }
    Q_ASSERT(_fiModel);
    QFileInfo fi = _fiModel->fileInfo(index);
    if (fi.isFile()) {
        emit fileActivated(fi.filePath());
    }
}


}
