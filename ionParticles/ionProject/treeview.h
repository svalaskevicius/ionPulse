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
#include <QKeyEvent>
#include "projectapi.h"
#include "treemodeladapter.h"

#include <ionParticles/ionEditor/editorapi.h>

namespace IonProject {

namespace Private {

class TreeModelAdapter;

class TreeView : public QTreeView
{
    Q_OBJECT
protected:
    QSharedPointer<TreeModelAdapter> _fiModel;

    virtual void keyPressEvent ( QKeyEvent * event );
public:
    explicit TreeView(QSharedPointer<TreeModelAdapter> dataModel, QWidget *parent = 0);
    virtual ~TreeView();
    void reset();
signals:
    void fileActivated(QString filename, int line);
    void filterKeyPressed(int key);
public slots:
    void expandToPath(QString path);
    void fileEditorFocused(IonEditor::Editor*);
protected slots:
    void onItemActivated(const QModelIndex &index);
    void onFilterTextChanged(const QString & text);
    void updateScrollArea(const QModelIndex &index);
protected:
    void _expandToPath(QString path, const QModelIndex &parent);

};


}
}

#endif // FILETREEWIDGET_H
