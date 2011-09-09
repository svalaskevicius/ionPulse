#include "treewidget.h"
#include "treemodel.h"
#include <QVBoxLayout>

namespace IonProject {
namespace Private {

TreeWidget::TreeWidget(QWidget *parent) :
    QTreeView(parent), _fiModel(NULL)
{
    _fiModel = new TreeModel("/Users/svalaskevicius/csDisk/warner.development.local");
    this->setModel(_fiModel);

    _filterInputField = new QLineEdit(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(5, 0, 5, 3);
    layout->addSpacing(100000);
    layout->addWidget(_filterInputField);
    setLayout(layout);

    connect(this, SIGNAL(activated( QModelIndex )), this, SLOT(onItemActivated( QModelIndex )));
    connect(_filterInputField, SIGNAL(textChanged( QString )), this, SLOT(onFilterTextChanged( QString )));
}

void TreeWidget::onItemActivated(const QModelIndex &index )
{
    /*if (!index.isValid()) {
        return;
    }
    Q_ASSERT(_fiModel);
    QFileInfo fi = _fiModel->fileInfo(index);
    if (fi.isFile()) {
        emit fileActivated(fi.filePath());
    }*/
}

void TreeWidget::keyPressEvent ( QKeyEvent * event ) {
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

void TreeWidget::onFilterTextChanged ( const QString & text ) {
    printf("FILTER: %s\n", text.toAscii().constData());
}

}

}
