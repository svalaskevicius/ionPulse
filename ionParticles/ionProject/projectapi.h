#pragma once

#include <QAbstractItemModel>
#include <ionHeart/plugin.h>


namespace IonProject {

class TreeBranch;

class TreeItem
{
public:
    virtual ~TreeItem() {}
    virtual QVariant data(int column) const = 0;
    virtual int getRowNr() = 0;
    virtual TreeBranch *parent() = 0;
    virtual int childrenCount() const = 0;
    virtual QList<TreeItem*> getChildren() const = 0;
    virtual void filter(QString const filter) = 0;
    virtual bool isVisible() const = 0;
    virtual QString getPath() const = 0;
};

class TreeBranch : virtual public TreeItem
{
public:
    virtual ~TreeBranch() {}

    virtual void appendChild(TreeItem *child) = 0;
    virtual TreeItem *getChild(int rowNr) = 0;
    virtual int childrenCount() const = 0;
    virtual int getChildRowNr(TreeItem *child) = 0;
    virtual void filter(QString const filter) = 0;
};

class TreeModelSource {
public:
    virtual TreeBranch * setupData() = 0;
    virtual QString getTitle() const = 0;
};


class TreeModel : public QAbstractItemModel {
public:
    virtual ~TreeModel() {}
    virtual void filter(QString filter) = 0;
    virtual QString getPath(const QModelIndex &index) const = 0;
    virtual QString getTitle() const = 0;
    virtual TreeBranch *getRoot() const = 0;
};

class ProjectPlugin : public IonHeart::BasicPlugin {
public:
    virtual void addTreeWidget(TreeModel *model) = 0;
    virtual void addTreeWidget(TreeModelSource *modelSource) = 0;
    virtual TreeModel *getProjectFileTreeModel() = 0;
    static QString name() {return "ionProject";}
};

}
