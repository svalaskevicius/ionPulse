#ifndef PROJECT_TREEITEM_H
#define PROJECT_TREEITEM_H

#include <QList>
#include <QVariant>

namespace IonProject {
namespace Private {

class TreeBranch;
class TreeItem;

class TreeItem
{
public:
    TreeItem(const QList<QVariant> &data, TreeBranch *parent = 0);
    virtual ~TreeItem();
    int columnCount() const;
    QVariant data(int column) const;
    int getRowNr();
    TreeItem *parent();
    virtual int childrenCount() const;

private:
    QList<QVariant> itemData;
    TreeBranch *parentItem;
};

class TreeBranch : public TreeItem
{
public:
    TreeBranch(const QList<QVariant> &data, TreeBranch *parent = 0);
    ~TreeBranch();

    void appendChild(TreeItem *getChild);
    TreeItem *getChild(int getRowNr);
    virtual int childrenCount() const;
    int getChildRowNr(TreeItem *getChild);
private:
    QList<TreeItem*> childItems;
};

}
}

#endif
