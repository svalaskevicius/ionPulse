/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PROJECT_TREEMODEL_H
#define PROJECT_TREEMODEL_H

#include <QModelIndex>
#include <QVariant>
#include "projectapi.h"

namespace IonProject {
namespace Private {

class DirectoryTreeSource : public TreeModelSource {
public:
    DirectoryTreeSource(const QString &initialDir)
        : initialDir(initialDir) {
    }
    DirectoryTreeSource()
        : initialDir("") {
    }
    virtual TreeItem * setupData();
    virtual QString getTitle() const {return "Project Browser";}
protected:
    void addDirectory(TreeItem *parent, QString directory);
private:
    QString initialDir;
};


class FileTreeModel : public IonProject::TreeModel
{
    Q_OBJECT

public:
    FileTreeModel(TreeModelSource &source);
    ~FileTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void filter(QString filter);
    TreeItem* getItem(const QModelIndex &index) const;
    QString getTitle() const {return modelTitle;}
    TreeItem *getRoot() const {return rootItem;}
    void setDirectoryTreeSource(TreeModelSource &source);
private:
    TreeItem *rootItem;
    QString modelTitle;
};

}
}

#endif
