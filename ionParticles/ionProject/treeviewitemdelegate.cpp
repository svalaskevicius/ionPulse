/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "treeviewitemdelegate.h"

#include <ionCore/shared.h>
#include <QApplication>


namespace IonProject {
namespace Private {

TreeViewItemDelegate::TreeViewItemDelegate(QSharedPointer<TreeModelAdapter> treeModel, QWidget *parent)
    : QStyledItemDelegate(parent), treeModel(treeModel), widget(parent)
{
}

void TreeViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    bool first = true;
    QVector<TreeItem *> items = treeModel->getRangeItems(index);
    int cnt = items.count();
    int i = 0;
    foreach (TreeItem *current, items) {
        if (!first) {
            widget->setProperty("itemclass", "separator");
            widget->style()->unpolish(widget);
            widget->style()->polish(widget);
            const char s[] = {(const char)0xc2, (const char)0xbb, 0x00};// right double angle
            opt.text = QString::fromUtf8(s);
            opt.rect.setLeft(opt.rect.left() + 4);
            opt.viewItemPosition = QStyleOptionViewItemV4::Middle;
            opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

            opt.rect.setLeft(opt.rect.left() + opt.fontMetrics.width(opt.text) + 4);

            if (i+1 < cnt) {
                opt.viewItemPosition = QStyleOptionViewItemV4::Middle;
            } else {
                opt.viewItemPosition = QStyleOptionViewItemV4::End;
            }
        } else {
            first = false;
            opt.viewItemPosition = QStyleOptionViewItemV4::Beginning;
        }
        widget->setProperty("itemclass", current->getItemClass());
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
        opt.text = current->data(0).toString();
        opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        opt.rect.setLeft(opt.rect.left() + opt.fontMetrics.width(opt.text));
        i++;
    }
}

QSize TreeViewItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize ret;
    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    QVector<TreeItem*> range = treeModel->getRangeItems(index);
    const char s[] = {(const char)0xc2, (const char)0xbb, 0x00};// right double angle
    ret.setWidth( (8+opt.fontMetrics.width(QString::fromUtf8(s)) ) * (range.size()) );
    ret.setHeight(opt.fontMetrics.height());
    foreach (TreeItem *current, range) {
        ret.setWidth(ret.width() + opt.fontMetrics.width(current->data(0).toString()));
    }
    return ret;
}



}
}
