/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "treeviewitemdelegate.h"

#include <ionHeart/shared.h>
#include <QApplication>


namespace IonProject {
namespace Private {

TreeViewItemDelegate::TreeViewItemDelegate(QSharedPointer<TreeModel> treeModel, QObject *parent)
    : QStyledItemDelegate(parent), treeModel(treeModel)
{
}

void TreeViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    bool first = true;
    foreach (TreeItem *current, treeModel->getRangeItems(index)) {
        if (!first) {
            const char s[] = {0xc2, 0xbb, 0x00};// right double angle
            opt.text = QString::fromUtf8(s);
            opt.rect.setLeft(opt.rect.left() + 4);
            QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
            opt.rect.setLeft(opt.rect.left() + opt.fontMetrics.width(opt.text) + 4);
        } else {
            first = false;
        }
        opt.text = current->data(0).toString();
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
        opt.rect.setLeft(opt.rect.left() + opt.fontMetrics.width(opt.text));
    }
}

QSize TreeViewItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize ret;
    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    QVector<TreeItem*> range = treeModel->getRangeItems(index);
    ret.setWidth( (8+opt.fontMetrics.width(opt.text) ) * (range.size() - 1) );
    ret.setHeight(opt.fontMetrics.height());
    foreach (TreeItem *current, range) {
        ret.setWidth(ret.width() + opt.fontMetrics.width(current->data(0).toString()));
    }
    return ret;
}



}
}
