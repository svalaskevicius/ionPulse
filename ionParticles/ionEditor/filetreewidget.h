#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeView>
#include <QFileSystemModel>
#include <ionHeart/layout.h>

namespace IonEditor {

class FileTreeWidget : public QTreeView, public IonHeart::IPanelWidget
{
    Q_OBJECT
protected:
    QFileSystemModel *_fiModel;
public:
    explicit FileTreeWidget(QWidget *parent = 0);
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelTitle() {return "File Browser";}
    virtual QString getPanelZone() {return "left";}

signals:
    void fileActivated(QString filename);
public slots:
protected slots:
    void onItemActivated(const QModelIndex &index );

};

}

#endif // FILETREEWIDGET_H
