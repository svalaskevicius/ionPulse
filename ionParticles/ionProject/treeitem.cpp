/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QStringList>
#include <stdexcept>

#include "treeitem.h"
#include <ionCore/shared.h>

namespace IonProject {

namespace Private {

TreeItemImpl::TreeItemImpl(QString itemClass, QString const name, QString const filterBy, QString const path, int line, TreeItem *parent)
    : itemClass(itemClass), childItems(), name(name), filterBy(filterBy), path(path), line(line), parentItem(parent)
{
    visible = true;
}
TreeItemImpl::~TreeItemImpl()
{
    qDeleteAll(childItems);
    childItems.clear();
}

void TreeItemImpl::clearChildren() {
    qDeleteAll(childItems);
    childItems.clear();
}


QVariant TreeItemImpl::data(int column) const
{
    if (0 == column) {
        return QVariant(name);
    }
    return QVariant();
}

TreeItem *TreeItemImpl::parent()
{
    return parentItem;
}

int TreeItemImpl::getRowNr()
{
    if (parentItem)
        return parentItem->getChildRowNr(this);

    return 0;
}

void TreeItemImpl::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeItemImpl::getVisibleChildByNr(int row)
{
    int current = 0;
    foreach (TreeItem* child, childItems) {
        if (child->isVisible()) {
            if (current == row) {
                return child;
            }
            current++;
        }
    }
    throw std::out_of_range("children size reached");
}

int TreeItemImpl::visibleChildrenCount() const
{
    int current = 0;
    foreach (TreeItem* child, childItems) {
        if (child->isVisible()) {
            current++;
        }
    }
    return current;
}

int TreeItemImpl::getChildRowNr(TreeItem *child)
{
    int current = 0;
    foreach (TreeItem* current_child, childItems) {
        if (current_child->isVisible()) {
            if (current_child == child) {
                return current;
            }
            current++;
        }
    }
    throw std::out_of_range("children size reached");
}

void TreeItemImpl::filter(QString const filter)
{
    filterByRegexp(createFilterRegexp(filter));
}

void TreeItemImpl::filterByRegexp(QRegExp const filter)
{
    visible = false;
    if (filterBy.length()) {
        visible = filter.indexIn(filterBy) >= 0;
    }
    if (visible) {
        setFullVisibility(true);
    } else {
        foreach (TreeItem* child, childItems) {
            child->filterByRegexp(filter);
            visible |= child->isVisible();
        }
    }
}

QRegExp TreeItemImpl::createFilterRegexp(QString const filter)
{
    QString pattern = "";
    foreach (const QChar &c, filter) {
        if (c != ' ') {
            pattern += c;
        } else {
            pattern += ".*";
        }
    }
    return QRegExp(pattern, Qt::CaseInsensitive);
}


void TreeItemImpl::setFullVisibility(bool visible)
{
    this->visible = visible;
    foreach (TreeItem* child, childItems) {
        child->setFullVisibility(visible);
    }
}

TreeItem* TreeItemFactoryImpl::createTreeItem(QString itemClass, QString const name, QString filterBy, QString const path, int const line, TreeItem *parent)
{
    return new Private::TreeItemImpl(itemClass, name, filterBy, path, line, parent);
}


}
}
