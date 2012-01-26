/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef TREEVIEWITEMDELEGATE_H
#define TREEVIEWITEMDELEGATE_H


#include "projectapi.h"

#include <QStyledItemDelegate>

namespace IonProject {
namespace Private {


class TreeViewItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TreeViewItemDelegate(QSharedPointer<TreeModel> treeModel, QObject *parent = 0);
    virtual ~TreeViewItemDelegate(){}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
protected:
    QSharedPointer<TreeModel> treeModel;
signals:
public slots:

};

}
}

#endif // TREEVIEWITEMDELEGATE_H
