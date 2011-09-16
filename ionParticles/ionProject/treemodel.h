/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PROJECT_TREEMODEL_H
#define PROJECT_TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

namespace IonProject {
namespace Private {

class TreeBranch;

class TreeModelSource {
public:
    virtual TreeBranch * setupData() = 0;
};

class DirectoryTreeSource : public TreeModelSource {
public:
    DirectoryTreeSource(const QString &initialDir)
        : initialDir(initialDir) {
    }
    virtual TreeBranch * setupData();
private:
    QString initialDir;
};


class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(TreeModelSource *source, QObject *parent = 0);
    ~TreeModel();

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

    QString getPath(const QModelIndex &index) const;
private:
    TreeBranch *rootItem;
};

}
}

#endif
