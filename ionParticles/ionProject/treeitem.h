/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PROJECT_TREEITEM_H
#define PROJECT_TREEITEM_H

#include <QList>
#include <QVariant>
#include "projectapi.h"

namespace IonProject {
namespace Private {

class TreeItemImpl : virtual public TreeItem
{
public:
    TreeItemImpl(QString itemClass, QString const name, QString const filterBy, QString const path, int line, TreeItem *parent);
    virtual ~TreeItemImpl();

    QVariant data(int column) const;
    int getRowNr();
    TreeItem *parent();
    virtual int visibleChildrenCount() const;
    virtual void filter(QString const filter);
    bool isVisible() const {return visible;}
    QString getPath() const {return path;}
    int getLine() const {return line;}
    virtual void setFullVisibility(bool visible);

    virtual void appendChild(TreeItem *item);
    virtual TreeItem *getVisibleChildByNr(int getRowNr);
    virtual QList<TreeItem*> &getChildren() {return childItems;}
    virtual void clearChildren();
    virtual int getChildRowNr(TreeItem *getVisibleChildByNr);
    virtual const QString &getItemClass() const {return itemClass;}
private:
    QString itemClass;
    QList<TreeItem*> childItems;
    QString name, filterBy, path;
    int line;
    TreeItem *parentItem;
protected:
    bool visible;
};

class TreeItemFactoryImpl : public TreeItemFactory {
public:
    virtual TreeItem* createTreeItem(QString itemClass, QString const name, QString filterBy, QString const path, int const line, TreeItem *parent);
};


}
}

#endif
