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
    virtual int getLine() const = 0;
};

class TreeBranch : virtual public TreeItem
{
public:
    virtual ~TreeBranch() {}

    virtual void appendChild(TreeItem *child) = 0;
    virtual TreeItem *getChild(int rowNr) = 0;
    virtual int getChildRowNr(TreeItem *child) = 0;
};

class TreeItemFactory {
public:
    virtual ~TreeItemFactory() {}
    virtual TreeItem* createTreeItem(QString const name, QString filterBy, QString const path, int const line, TreeBranch *parent) = 0;
    virtual TreeBranch* createTreeBranch(QString const name, QString const path, int const line, TreeBranch *parent) = 0;
};

class TreeModelSource {
public:
    virtual ~TreeModelSource() {}
    virtual TreeBranch * setupData() = 0;
    virtual QString getTitle() const = 0;
};


class TreeModel : public QAbstractItemModel {
public:
    virtual ~TreeModel() {}
    virtual void filter(QString filter) = 0;
    virtual TreeItem* getItem(const QModelIndex &index) const = 0;
    virtual QString getTitle() const = 0;
    virtual TreeBranch *getRoot() const = 0;
};

class ProjectPlugin : public IonHeart::BasicPlugin {
public:
    virtual void addTreeWidget(TreeModel *model) = 0;
    virtual void addTreeWidget(TreeModelSource *modelSource) = 0;
    virtual TreeItemFactory *createTreeItemFactory() = 0;
    virtual TreeModel *getProjectFileTreeModel() = 0;
    static QString name() {return "ionProject";}
};

}
