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
#include "filetreemodel.h"

namespace IonProject {
namespace Private {

FileTreeModel::FileTreeModel(TreeModelSource &source)
{
    rootItem = source.setupData();
    modelTitle = source.getTitle();
}

FileTreeModel::~FileTreeModel()
{
    delete rootItem;
}

void FileTreeModel::setDirectoryTreeSource(TreeModelSource &source)
{
    if (rootItem) {
        delete rootItem;
    }
    rootItem = source.setupData();
    modelTitle = source.getTitle();
    reset();
}

int FileTreeModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant FileTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItemImpl *item = static_cast<TreeItemImpl*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags FileTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant FileTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex FileTreeModel::index(int row, int column, const QModelIndex &parent)
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

QModelIndex FileTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->getRowNr(), 0, parentItem);
}

int FileTreeModel::rowCount(const QModelIndex &parent) const
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

void FileTreeModel::filter(QString filter) {
    rootItem->filter(filter);
    reset();
}

TreeItem* FileTreeModel::getItem(const QModelIndex &index) const {
    if (!index.isValid()) {
        throw std::runtime_error("index is invalid");
    }

    TreeItemImpl *item = static_cast<TreeItemImpl*>(index.internalPointer());
    return item;
}





TreeItem *DirectoryTreeSource::setupData()
{
    TreeItemImpl* root = new TreeItemImpl("Name", "", "", -1, NULL);

    if (initialDir.length()) {
        addDirectory(root, initialDir);
    }

    return root;
}

void DirectoryTreeSource::addDirectory(TreeItem *parent, QString directory)
{
    QList<TreeItem*> parents;
    QList<QString> directoryNames;

    parents << parent;
    directoryNames << directory;

    while (directoryNames.count()) {
        QString currentDirName = directoryNames.back();
        directoryNames.pop_back();
        QDir currentDir(currentDirName);
        TreeItem* currentTreeItemsParent = parents.last();
        parents.pop_back();

        foreach (QString subDirName, currentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)) {
            QString fullPath = currentDir.absolutePath()+"/"+subDirName;
            TreeItem* newTreeItem = new TreeItemImpl(subDirName, subDirName, fullPath, -1, currentTreeItemsParent);
            currentTreeItemsParent->appendChild(newTreeItem);

            directoryNames.append(fullPath);
            parents << newTreeItem;
        }

        foreach (QString fileName, currentDir.entryList(QDir::Files, QDir::Name)) {
            QString fullPath = currentDir.absolutePath()+"/"+fileName;
            currentTreeItemsParent->appendChild(new TreeItemImpl(fileName, fileName, fullPath, -1, currentTreeItemsParent));
        }
    }
}

}
}
