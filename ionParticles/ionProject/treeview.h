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
    // keeps right and bottom positions as map keys for a faster lookup
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
    Q_INVOKABLE IonProject::TreeItem *treeItemAt(const QPoint &pos);
    Q_INVOKABLE IonProject::TreeItem *getCurrentHoveredItem() const {return currentHoveredItem;}
    Q_INVOKABLE void saveExpandState();
    Q_INVOKABLE void restoreExpandState();
    Q_INVOKABLE void updateProjectNode(QModelIndex parent);
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
private:
    QList<TreeItem *> _savedExpandState;
    QList<TreeItem *> _savedCurrentItems;
};


}
}

#endif // FILETREEWIDGET_H
