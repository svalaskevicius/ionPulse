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

TreeItemImpl::TreeItemImpl(QString const name, QString const filterBy, QString const path, int line, TreeItem *parent)
    : childItems(), name(name), filterBy(filterBy), path(path), line(line), parentItem(parent)
{
    visible = true;
}
TreeItemImpl::~TreeItemImpl()
{
    qDeleteAll(childItems);
}



QVariant TreeItemImpl::data(int column) const
{
    if (0 == column) {
        return QVariant(name);
    }
    return QVariant();
}

TreeItem *TreeItemImpl::parent()
{
    return parentItem;
}

int TreeItemImpl::getRowNr()
{
    if (parentItem)
        return parentItem->getChildRowNr(this);

    return 0;
}

void TreeItemImpl::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeItemImpl::getChild(int row)
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

int TreeItemImpl::childrenCount() const
{
    int current = 0;
    foreach (TreeItem* child, childItems) {
        if (child->isVisible()) {
            current++;
        }
    }
    return current;
}

int TreeItemImpl::getChildRowNr(TreeItem *child)
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

void TreeItemImpl::filter(QString const filter)
{
    if (filterBy.length()) {
        visible = filterBy.toLower().contains(filter.toLower());
    } else {
        visible = false;
        foreach (TreeItem* child, childItems) {
            child->filter(filter);
            visible |= child->isVisible();
        }
    }
}

TreeItem* TreeItemFactoryImpl::createTreeItem(QString const name, QString filterBy, QString const path, int const line, TreeItem *parent)
{
    return new Private::TreeItemImpl(name, filterBy, path, line, parent);
}


}
}
