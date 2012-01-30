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


class TreeModelAdapter : public IonProject::TreeModel
{
    Q_OBJECT

public:
    TreeModelAdapter(QSharedPointer<TreeModelSource> source);
    ~TreeModelAdapter();

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
    TreeItem* getDirectItem(const QModelIndex &index) const;
    QVector<TreeItem *> getRangeItems(const QModelIndex &index) const;
    QString getTitle() const {return modelTitle;}
    TreeItem *getRoot() const {return rootItem;}
    void setDirectoryTreeSource(QSharedPointer<TreeModelSource> source);
    void updateFromSource();
private:
    TreeItem *rootItem;
    QString modelTitle;
    QSharedPointer<TreeModelSource> source;
};

}
}

#endif
