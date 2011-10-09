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
    TreeItemImpl(QString const name, QString const path, TreeBranch *parent);
    virtual ~TreeItemImpl();
    QVariant data(int column) const;
    int getRowNr();
    TreeBranch *parent();
    virtual int childrenCount() const;
    virtual void filter(QString const filter);
    bool isVisible() const {return visible;}
    QString getPath() const {return path;}
private:
    QString name, path;
    TreeBranch *parentItem;
protected:
    bool visible;
};

class TreeBranchImpl : public TreeItemImpl, public TreeBranch
{
public:
    TreeBranchImpl(QString const name, QString const path, TreeBranch *parent);
    ~TreeBranchImpl();

    void appendChild(TreeItem *getChild);
    TreeItem *getChild(int getRowNr);
    virtual int childrenCount() const;
    int getChildRowNr(TreeItem *getChild);
    virtual void filter(QString const filter);
private:
    QList<TreeItem*> childItems;
};

}
}

#endif
