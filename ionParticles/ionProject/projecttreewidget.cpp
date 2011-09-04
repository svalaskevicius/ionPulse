#include "projecttreewidget.h"
#include <QVBoxLayout>

namespace IonProject {

namespace Private {

ProjectTreeWidget::ProjectTreeWidget(QWidget *parent) :
    QTreeView(parent), _fiModel(NULL)
{
    _fiModel = new QFileSystemModel(this);
    _fiModel->setRootPath(QDir::homePath());
    this->setModel(_fiModel);
    this->setRootIndex(_fiModel->index(QDir::homePath()));
    for (int i=1;i<_fiModel->columnCount();i++) {
        this->setColumnHidden(i, true);
    }
    _filterInputField = new QLineEdit(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(5, 0, 5, 3);
    layout->addSpacing(100000);
    layout->addWidget(_filterInputField);

    setLayout(layout);

   connect(this, SIGNAL(activated( QModelIndex )), this, SLOT(onItemActivated( QModelIndex )));
}

void ProjectTreeWidget::onItemActivated(const QModelIndex &index )
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

void ProjectTreeWidget::keyPressEvent ( QKeyEvent * event ) {
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit activated(currentIndex());
            break;
        default:
            QTreeView::keyPressEvent(event);
    }
}

}

}
