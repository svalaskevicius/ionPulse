#include <QStringList>
#include <stdexcept>

#include "treeitem.h"

namespace IonProject {

namespace Private {

TreeItem::TreeItem(const QList<QVariant> &data, TreeBranch *parent)
{
    parentItem = parent;
    itemData = data;
    visible = true;
}
TreeItem::~TreeItem()
{
}


int TreeItem::columnCount() const
{
    return itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return itemData.value(column);
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

int TreeItem::getRowNr()
{
    if (parentItem)
        return parentItem->getChildRowNr(this);

    return 0;
}
int TreeItem::childrenCount() const
{
       return 0;
}

void TreeItem::setFilter(QString const filter)
{
    visible = itemData[1].toString().toLower().contains(filter.toLower());
}





TreeBranch::TreeBranch(const QList<QVariant> &data, TreeBranch *parent)
    :TreeItem(data, parent), childItems()
{
}
TreeBranch::~TreeBranch()
{
    qDeleteAll(childItems);
}

void TreeBranch::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeBranch::getChild(int row)
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
//    return childItems.value(row);
}

int TreeBranch::childrenCount() const
{
    int current = 0;
    foreach (TreeItem* child, childItems) {
        if (child->isVisible()) {
            current++;
        }
    }
    return current;
//    return childItems.count();
}
int TreeBranch::getChildRowNr(TreeItem *child)
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
    //return childItems.indexOf(child);
}
void TreeBranch::setFilter(QString const filter)
{
    visible = false;
    foreach (TreeItem* child, childItems) {
        child->setFilter(filter);
        visible |= child->isVisible();
    }
}


}
}
