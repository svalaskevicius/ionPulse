#include <QStringList>

 #include "treeitem.h"

namespace IonProject {

namespace Private {

TreeItem::TreeItem(const QList<QVariant> &data, TreeBranch *parent)
{
    parentItem = parent;
    itemData = data;
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
    return childItems.value(row);
}

int TreeBranch::childrenCount() const
{
    return childItems.count();
}
int TreeBranch::getChildRowNr(TreeItem *child)
{
    return childItems.indexOf(child);
}

}
}
