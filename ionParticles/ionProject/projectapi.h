#pragma once

#include <QAbstractItemModel>


namespace IonProject {

class TreeModel : public QAbstractItemModel {
public:
    virtual ~TreeModel() {}
    virtual void filter(QString filter) = 0;
    virtual QString getPath(const QModelIndex &index) const = 0;
};

}
