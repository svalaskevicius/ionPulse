/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QStringList>
#include <stdexcept>

#include "treeitem.h"

namespace IonProject {

namespace Private {

TreeItemImpl::TreeItemImpl(QString const name, QString const path, TreeBranch *parent)
    : name(name), path(path), parentItem(parent)
{
    visible = true;
}
TreeItemImpl::~TreeItemImpl()
{
}



QVariant TreeItemImpl::data(int column) const
{
    if (0 == column) {
        return QVariant(name);
    }
    return QVariant();
}

TreeBranch *TreeItemImpl::parent()
{
    return parentItem;
}

int TreeItemImpl::getRowNr()
{
    if (parentItem)
        return parentItem->getChildRowNr(this);

    return 0;
}
int TreeItemImpl::childrenCount() const
{
       return 0;
}

void TreeItemImpl::filter(QString const filter)
{
    visible = path.toLower().contains(filter.toLower());
}





TreeBranchImpl::TreeBranchImpl(QString const name, QString const path, TreeBranch *parent)
    :TreeItemImpl(name, path, parent), childItems()
{
}
TreeBranchImpl::~TreeBranchImpl()
{
    qDeleteAll(childItems);
}

void TreeBranchImpl::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeBranchImpl::getChild(int row)
{
    int current = 0;
    foreach (TreeItem* child, childItems) {
        if (child->isVisible()) {
            if (current == row) {
                return child;
            }
            current++;
        }
    }
    throw std::out_of_range("children size reached");
}

int TreeBranchImpl::childrenCount() const
{
    int current = 0;
    foreach (TreeItem* child, childItems) {
        if (child->isVisible()) {
            current++;
        }
    }
    return current;
}

int TreeBranchImpl::getChildRowNr(TreeItem *child)
{
    int current = 0;
    foreach (TreeItem* cchild, childItems) {
        if (cchild->isVisible()) {
            if (cchild == child) {
                return current;
            }
            current++;
        }
    }
    throw std::out_of_range("children size reached");
}

void TreeBranchImpl::filter(QString const filter)
{
    visible = false;
    foreach (TreeItem* child, childItems) {
        child->filter(filter);
        visible |= child->isVisible();
    }
}

TreeItem* TreeItemFactoryImpl::createTreeItem(QString const name, QString const path, TreeBranch *parent)
{
    return new Private::TreeItemImpl(name, path, parent);
}

TreeBranch* TreeItemFactoryImpl::createTreeBranch(QString const name, QString const path, TreeBranch *parent)
{
    return new Private::TreeBranchImpl(name, path, parent);
}

}
}
