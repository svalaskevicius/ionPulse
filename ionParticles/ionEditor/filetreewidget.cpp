#include "filetreewidget.h"
#include <QFileSystemModel>

namespace IonEditor {


FileTreeWidget::FileTreeWidget(QWidget *parent) :
    QTreeView(parent)
{
    // todo: add filtering, and move qtreeview to subelement
    QFileSystemModel *model = new QFileSystemModel(this);
    model->setNameFilters(QStringList() << "*.php");
    model->setNameFilterDisables(false);
    model->setRootPath(QDir::homePath());
    this->setModel(model);
    this->setRootIndex(model->index(QDir::homePath()));
    for (int i=1;i<model->columnCount();i++) {
        this->setColumnHidden(i, true);
    }
}


}
