#pragma once

#include <QAbstractItemModel>
#include <ionHeart/plugin.h>

#include <boost/function.hpp>


namespace IonProject {

class TreeItem
{
public:
    virtual ~TreeItem() {}
    virtual QVariant data(int column) const = 0;
    virtual int getRowNr() = 0;
    virtual TreeItem *parent() = 0;
    virtual int childrenCount() const = 0;
    virtual QList<TreeItem*> getChildren() const = 0;
    virtual void filter(QString const filter) = 0;
    virtual bool isVisible() const = 0;
    virtual QString getPath() const = 0;
    virtual int getLine() const = 0;
    virtual void appendChild(TreeItem *child) = 0;
    virtual TreeItem *getChild(int rowNr) = 0;
    virtual int getChildRowNr(TreeItem *child) = 0;
};

class TreeItemFactory {
public:
    virtual ~TreeItemFactory() {}
    virtual TreeItem* createTreeItem(QString const name, QString filterBy, QString const path, int const line, TreeItem *parent) = 0;
};

class TreeModelSource {
public:
    virtual ~TreeModelSource() {}
    virtual TreeItem * setupData() = 0;
    virtual QString getTitle() const = 0;
};


class TreeModel : public QAbstractItemModel {
public:
    virtual ~TreeModel() {}
    virtual void filter(QString filter) = 0;
    virtual TreeItem* getItem(const QModelIndex &index) const = 0;
    virtual QString getTitle() const = 0;
    virtual TreeItem *getRoot() const = 0;
    virtual void setDirectoryTreeSource(TreeModelSource &source) = 0;
};

class ProjectPlugin : public IonHeart::BasicPlugin {
public:
    virtual void addTreeWidget(QSharedPointer<TreeModel> model) = 0;
    virtual void addTreeWidget(TreeModelSource *modelSource) = 0;
    virtual QSharedPointer<TreeItemFactory> createTreeItemFactory() = 0;
    virtual QSharedPointer<TreeModel> getProjectFileTreeModel() = 0;
    static QString name() {return "ionProject";}
    virtual const boost::function<QSharedPointer<TreeModelSource> (QString dirname)> &getTreeModelSourceFactory() = 0;
    virtual void setTreeModelSourceFactory(boost::function<QSharedPointer<TreeModelSource> (QString dirname)> factory) = 0;
};

}
