#include <QStringList>
#include <stdexcept>

#include "treeitem.h"

namespace IonProject {

namespace Private {

TreeItem::TreeItem(QString const name, QString const path, TreeBranch *parent)
    : name(name), path(path), parentItem(parent)
{
    visible = true;
}
TreeItem::~TreeItem()
{
}



QVariant TreeItem::data(int column) const
{
    if (0 == column) {
        return QVariant(name);
    }
    return QVariant();
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

void TreeItem::filter(QString const filter)
{
    visible = path.toLower().contains(filter.toLower());
}





TreeBranch::TreeBranch(QString const name, QString const path, TreeBranch *parent)
    :TreeItem(name, path, parent), childItems()
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
void TreeBranch::filter(QString const filter)
{
    visible = false;
    foreach (TreeItem* child, childItems) {
        child->filter(filter);
        visible |= child->isVisible();
    }
}


}
}
