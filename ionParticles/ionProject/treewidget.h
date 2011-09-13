/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PROJECT_TREEWIDGET_H
#define PROJECT_TREEWIDGET_H

#include <QTreeView>
#include <QAbstractItemModel>
#include <ionHeart/layout.h>
#include <QKeyEvent>
#include <QLineEdit>

namespace IonProject {

namespace Private {

class TreeModel;

class TreeWidget : public QTreeView, public IonHeart::PanelWidget
{
    Q_OBJECT
protected:
    TreeModel *_fiModel;
    QLineEdit *_filterInputField;

    virtual void keyPressEvent ( QKeyEvent * event );
public:
    explicit TreeWidget(QWidget *parent = 0);
    virtual QWidget *getWidget() {return this;}
    virtual QString getPanelTitle() {return "Project Browser";}
    virtual QString getPanelZone() {return "left";}

signals:
    void fileActivated(QString filename);
public slots:
protected slots:
    void onItemActivated(const QModelIndex &index );
    void onFilterTextChanged ( const QString & text );
};


}
}

#endif // FILETREEWIDGET_H
