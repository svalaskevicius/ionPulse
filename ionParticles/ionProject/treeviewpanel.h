#ifndef TREEVIEWPANEL_H
#define TREEVIEWPANEL_H

#include <QWidget>
#include <QLineEdit>

#include <ionParticles/ionLayout/layoutapi.h>


#include "treeview.h"

namespace IonProject {
namespace Private {


class TreeViewPanel : public QWidget, public IonLayout::PanelWidget
{
    Q_OBJECT
protected:
    QLineEdit *_filterInputField;
    TreeView *_treeView;
    QSharedPointer<TreeModelAdapter> _dataModel;
public:
    explicit TreeViewPanel(QSharedPointer<TreeModelAdapter> dataModel, QWidget *parent = 0);
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelTitle() {return _dataModel->getTitle();}
    virtual QString getPanelZone() {return "left";}

    TreeView *getTreeView(){return _treeView;}

signals:

public slots:
    void filterKey(int key);
    void applyFilter();
};

}
}

#endif // TREEVIEWPANEL_H
