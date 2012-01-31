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

TreeModelAdapter::TreeModelAdapter(QSharedPointer<TreeModelSource> source)
    : rootItem(NULL), source(source)
{
    updateFromSource();
}

TreeModelAdapter::~TreeModelAdapter()
{
    if (rootItem) {
        delete rootItem;
        rootItem = NULL;
    }
}

void TreeModelAdapter::setDirectoryTreeSource(QSharedPointer<TreeModelSource> source)
{
    if (rootItem) {
        delete rootItem;
        rootItem = NULL;
    }
    this->source = source;
    updateFromSource();
}

void TreeModelAdapter::updateFromSource()
{
    TreeItem *oldRoot = rootItem;
    rootItem = source->setupData();
    if (oldRoot && (rootItem != oldRoot)) {
        delete oldRoot;
    }
    modelTitle = source->getTitle();
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

    return getItem(index)->data(index.column());
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
        parentItem = getItem(parent);

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

int TreeModelAdapter::rowCount(const QModelIndex &index) const
{
    TreeItem *item;
    if (index.column() > 0)
        return 0;

    if (!index.isValid())
        item = rootItem;
    else
        item = getItem(index);

    return item->childrenCount();
}

void TreeModelAdapter::filter(QString filter) {
    rootItem->filter(filter);
    reset();
}

TreeItem* TreeModelAdapter::getItem(const QModelIndex &index) const
{
    TreeItem *item = getDirectItem(index);

    while (1 == item->childrenCount()) {
        item = item->getChild(0);
    }
    return item;
}

TreeItem* TreeModelAdapter::getDirectItem(const QModelIndex &index) const
{
    if (!index.isValid()) {
        throw std::runtime_error("index is invalid");
    }

    if (TreeItem *item = static_cast<TreeItem*>(index.internalPointer())) {
        return item;
    }

    throw std::runtime_error("index contains invalid pointer");
}

QVector<TreeItem *> TreeModelAdapter::getRangeItems(const QModelIndex &index) const
{
    TreeItem *parent = getDirectItem(index);
    TreeItem *current = getItem(index);

    QVector<TreeItem *> itemPath;

    itemPath.prepend(current);
    while (current != parent) {
        current = current->parent();
        itemPath.prepend(current);
    }
    return itemPath;
}





}
}
