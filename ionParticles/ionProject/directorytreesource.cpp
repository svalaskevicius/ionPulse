/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QtGui>
#include <stdexcept>

#include "treeitem.h"
#include "directorytreesource.h"

namespace IonProject {
namespace Private {

struct MissingFilesCleaner {
public:
    QStringList &dirnames, &filenames;
    MissingFilesCleaner(QStringList &dirnames, QStringList &filenames)
        : dirnames(dirnames), filenames(filenames)
    {
    }
    void clean(QString prefix, QList<TreeItem*> &children) {
        QList<TreeItem*>::iterator it = children.begin();
        while (it != children.end()) {
            it = processIterator(prefix, children, it);
        }
    }
private:
    inline QList<TreeItem*>::iterator &processIterator(QString prefix, QList<TreeItem*> &children, QList<TreeItem*>::iterator &it) {
        if (TREESOURCE_CLASS_DIR == (*it)->getItemClass()) {
            if (dirnames.end() == findPathNode(prefix, "/", (*it)->getPath(), dirnames)) {
                delete *it;
                it = children.erase(it);
                return it;
            }
        } else if (TREESOURCE_CLASS_FILE == (*it)->getItemClass()) {
            if (filenames.end() == findPathNode(prefix, "", (*it)->getPath(), filenames)) {
                delete *it;
                it = children.erase(it);
                return it;
            }
        }
        return ++it;
    }
    inline QStringList::iterator findPathNode(QString prefix, QString suffix, QString path, QStringList &paths)
    {
        for (QStringList::iterator it = paths.begin(); it!=paths.end(); it++) {
            if (prefix + (*it) + suffix == path) {
                return it;
            }
        }
        return paths.end();
    }
};

TreeItem *DirectoryTreeSource::setupData(QString pathFilter)
{
    if (!root) {
        root = new TreeItemImpl(TREESOURCE_CLASS_DIR, "Name", "", initialDir, -1, NULL);
    }

    if (initialDir.length()) {
        addDirectory(root, pathFilter);
    }

    return root;
}

void DirectoryTreeSource::addDirectory(TreeItem *parent, QString pathFilter)
{
    QList<TreeItem*> parents;
    parents << parent;

    while (parents.count()) {
        TreeItem* currentTreeItemsParent = parents.last();
        parents.pop_back();

        if ((pathFilter == "") || pathFilter.startsWith(currentTreeItemsParent->getPath())) {
            _updateDirectory(currentTreeItemsParent, parents);
        }
    }
}

void DirectoryTreeSource::_updateDirectory(TreeItem* currentTreeItemsParent, QList<TreeItem*> &parents)
{
    QSharedPointer<DirectoryTreeSource::DirectoryInfo> currentDir = _getDir(currentTreeItemsParent->getPath());
    QStringList dirnames = currentDir->dirnames();
    QStringList filenames = currentDir->filenames();

    MissingFilesCleaner cleaner(dirnames, filenames);
    cleaner.clean(currentDir->absolutePath()+"/", currentTreeItemsParent->getChildren());

    foreach (QString subDirName, dirnames) {
        QString fullPath = currentDir->absolutePath()+"/"+subDirName+"/";
        TreeItem *treeItem = findChildForPath(currentTreeItemsParent, fullPath);
        if (!treeItem) {
            treeItem = new TreeItemImpl(TREESOURCE_CLASS_DIR, subDirName, subDirName, fullPath, -1, currentTreeItemsParent);
            currentTreeItemsParent->appendChild(treeItem);
        }
        parents << treeItem;
    }

    foreach (QString fileName, filenames) {
        QString fullPath = currentDir->absolutePath()+"/"+fileName;
        if (!findChildForPath(currentTreeItemsParent, fullPath)) {
            currentTreeItemsParent->appendChild(new TreeItemImpl(TREESOURCE_CLASS_FILE, fileName, fileName, fullPath, -1, currentTreeItemsParent));
        }
    }
}

TreeItem *DirectoryTreeSource::findChildForPath(TreeItem *node, QString path)
{
    for (
         QList<TreeItem*>::const_iterator it = node->getChildren().begin();
         it != node->getChildren().end();
         it++
    ) {
        if ((*it)->getPath() == path) {
            return *it;
        }
    }
    return NULL;
}

QSharedPointer<DirectoryTreeSource::DirectoryInfo> DirectoryTreeSource::_getDir(const QString path)
{
    return QSharedPointer<DirectoryTreeSource::DirectoryInfo>(new DirectoryInfo(path));
}

}
}
