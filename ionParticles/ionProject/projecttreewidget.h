#ifndef PROJECTTREEWIDGET_H
#define PROJECTTREEWIDGET_H

#include <QTreeView>
#include <QFileSystemModel>
#include <ionHeart/layout.h>
#include <QKeyEvent>
#include <QLineEdit>

namespace IonProject {

namespace Private {

class ProjectTreeWidget : public QTreeView, public IonHeart::PanelWidget
{
    Q_OBJECT
protected:
    QFileSystemModel *_fiModel;
    QLineEdit *_filterInputField;

    virtual void keyPressEvent ( QKeyEvent * event );
public:
    explicit ProjectTreeWidget(QWidget *parent = 0);
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelTitle() {return "Project Browser";}
    virtual QString getPanelZone() {return "left";}

signals:
    void fileActivated(QString filename);
public slots:
protected slots:
    void onItemActivated(const QModelIndex &index );

};
}
}

#endif // FILETREEWIDGET_H
