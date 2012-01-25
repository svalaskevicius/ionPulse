/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QtGui>
#include <QDir>
#include <stdexcept>

#include "treeitem.h"
#include "treemodeladapter.h"

namespace IonProject {
namespace Private {

TreeModelAdapter::TreeModelAdapter(TreeModelSource &source)
{
    rootItem = source.setupData();
    modelTitle = source.getTitle();
}

TreeModelAdapter::~TreeModelAdapter()
{
    delete rootItem;
}

void TreeModelAdapter::setDirectoryTreeSource(TreeModelSource &source)
{
    if (rootItem) {
        delete rootItem;
    }
    rootItem = source.setupData();
    modelTitle = source.getTitle();
    reset();
}

int TreeModelAdapter::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant TreeModelAdapter::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItemImpl *item = static_cast<TreeItemImpl*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModelAdapter::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModelAdapter::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModelAdapter::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->getChild(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModelAdapter::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->getRowNr(), 0, parentItem);
}

int TreeModelAdapter::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childrenCount();
}

void TreeModelAdapter::filter(QString filter) {
    rootItem->filter(filter);
    reset();
}

TreeItem* TreeModelAdapter::getItem(const QModelIndex &index) const {
    if (!index.isValid()) {
        throw std::runtime_error("index is invalid");
    }

    TreeItemImpl *item = static_cast<TreeItemImpl*>(index.internalPointer());
    return item;
}





}
}
