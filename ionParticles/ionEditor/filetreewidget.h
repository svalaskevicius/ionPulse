#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeView>
#include <ionHeart/layout.h>

namespace IonEditor {

class FileTreeWidget : public QTreeView, public IonHeart::IPanelWidget
{
    Q_OBJECT
public:
    explicit FileTreeWidget(QWidget *parent = 0);
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelTitle() {return "File Browser";}
    virtual QString getPanelZone() {return "left";}

signals:

public slots:

};

}

#endif // FILETREEWIDGET_H
