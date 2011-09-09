#include <QtGui>
#include <QDir>

#include "treeitem.h"
#include "treemodel.h"

namespace IonProject {
namespace Private {

TreeModel::TreeModel(const QString &initialDir, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Name";
    rootItem = new TreeBranch(rootData);
    setupModelData(initialDir, rootItem);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeBranch *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeBranch*>(parent.internalPointer());

    TreeItem *childItem = parentItem->getChild(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->getRowNr(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
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

void TreeModel::setupModelData(const QString &initialDir, TreeBranch *parent)
{
    QList<TreeBranch*> parents;
    QList<QString> directoryNames;
    parents << parent;
    directoryNames << initialDir;

    while (directoryNames.count()) {
        QString currentDirName = directoryNames.back();
        directoryNames.pop_back();
        QDir currentDir(currentDirName);
        TreeBranch* currentTreeItemsParent = parents.last();
        parents.pop_back();

        foreach (QString subDirName, currentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)) {
            TreeBranch* newTreeItem = new TreeBranch(QList<QVariant>() << subDirName, currentTreeItemsParent);
            currentTreeItemsParent->appendChild(newTreeItem);

            directoryNames.append(currentDir.absolutePath()+"/"+subDirName);
            parents << newTreeItem;
        }

        foreach (QString fileName, currentDir.entryList(QDir::Files, QDir::Name)) {
            currentTreeItemsParent->appendChild(new TreeItem(QList<QVariant>() << fileName, currentTreeItemsParent));
        }
    }
}

}
}
