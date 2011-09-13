/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PROJECT_TREEITEM_H
#define PROJECT_TREEITEM_H

#include <QList>
#include <QVariant>

namespace IonProject {
namespace Private {

class TreeBranch;
class TreeItem;

struct TreeItem
{
public:
    TreeItem(QString const name, QString const path, TreeBranch *parent);
    virtual ~TreeItem();
    QVariant data(int column) const;
    int getRowNr();
    TreeItem *parent();
    virtual int childrenCount() const;
    virtual void filter(QString const filter);
    bool isVisible() const {return visible;}
    QString getPath() const {return path;}
private:
    QString name, path;
    TreeBranch *parentItem;
protected:
    bool visible;
};

struct TreeBranch : public TreeItem
{
public:
    TreeBranch(QString const name, QString const path, TreeBranch *parent);
    ~TreeBranch();

    void appendChild(TreeItem *getChild);
    TreeItem *getChild(int getRowNr);
    virtual int childrenCount() const;
    int getChildRowNr(TreeItem *getChild);
    virtual void filter(QString const filter);
private:
    QList<TreeItem*> childItems;
};

}
}

#endif
