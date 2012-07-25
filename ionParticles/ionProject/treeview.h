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
public:
    typedef struct {
        QRect rect;
        TreeItem *item;
    } PositionedItem;
protected:
    QSharedPointer<TreeModelAdapter> _fiModel;
    typedef QMap<int, QMap<int, PositionedItem> > ItemPositionCache;
    ItemPositionCache _itemPositionCache;

    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );
    TreeItem *currentHoveredItem;

    PositionedItem _findPositionedItemAt(const QPoint &pos) const;
public:
    explicit TreeView(QSharedPointer<TreeModelAdapter> dataModel, QWidget *parent = 0);
    virtual ~TreeView();
    void reset();
    TreeItem *treeItemAt(const QPoint &pos);
    TreeItem *getCurrentHoveredItem() const {return currentHoveredItem;}
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
    bool _isModelIndexDescendant(const QModelIndex &parent, const QModelIndex &descendantInQuestion);

};


}
}

#endif // FILETREEWIDGET_H
