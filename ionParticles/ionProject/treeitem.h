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
    TreeItemImpl(QString const name, QString const filterBy, QString const path, int line, TreeBranch *parent);
    virtual ~TreeItemImpl();
    QVariant data(int column) const;
    int getRowNr();
    TreeBranch *parent();
    virtual int childrenCount() const;
    virtual QList<TreeItem*> getChildren() const {return QList<TreeItem*>();}
    virtual void filter(QString const filter);
    bool isVisible() const {return visible;}
    QString getPath() const {return path;}
    int getLine() const {return line;}

private:
    QString name, filterBy, path;
    int line;
    TreeBranch *parentItem;
protected:
    bool visible;
};

class TreeBranchImpl : virtual public TreeBranch, public TreeItemImpl
{
public:
    TreeBranchImpl(QString const name, QString const path, int line, TreeBranch *parent);
    ~TreeBranchImpl();

    virtual void appendChild(TreeItem *getChild);
    virtual TreeItem *getChild(int getRowNr);
    virtual int childrenCount() const;
    virtual QList<TreeItem*> getChildren() const {return childItems;}
    virtual int getChildRowNr(TreeItem *getChild);
    virtual void filter(QString const filter);
private:
    QList<TreeItem*> childItems;
};

class TreeItemFactoryImpl : public TreeItemFactory {
public:
    virtual TreeItem* createTreeItem(QString const name, QString filterBy, QString const path, int const line, TreeBranch *parent);
    virtual TreeBranch* createTreeBranch(QString const name, QString const path, int const line, TreeBranch *parent);
};


}
}

#endif
