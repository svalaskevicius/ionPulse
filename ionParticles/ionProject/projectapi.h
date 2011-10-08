#pragma once

#include <QAbstractItemModel>
#include <ionHeart/plugin.h>


namespace IonProject {

class TreeModel : public QAbstractItemModel {
public:
    virtual ~TreeModel() {}
    virtual void filter(QString filter) = 0;
    virtual QString getPath(const QModelIndex &index) const = 0;
};

class ProjectPlugin : public IonHeart::BasicPlugin {
public:
    virtual TreeModel *getProjectFileTreeModel() = 0;
};

}
