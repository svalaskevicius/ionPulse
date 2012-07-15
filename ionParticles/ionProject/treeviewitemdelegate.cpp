/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "treeviewitemdelegate.h"

#include <ionCore/shared.h>
#include <QApplication>
#include <QLabel>
#include <QPainter>

namespace IonProject {
namespace Private {

TreeViewItemDelegate::TreeViewItemDelegate(QSharedPointer<TreeModelAdapter> treeModel, TreeView *parent)
    : QStyledItemDelegate(parent), treeModel(treeModel), widget(parent)
{
}

void TreeViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    opt.text = "";
    opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QLabel renderer;
    if (QStyle::State_Selected == (QStyle::State_Selected & opt.state)) {
        renderer.setProperty("selected", "line");
    }

    renderer.setProperty("type", "tree-item-text");
    renderer.setStyleSheet(widget->styleSheet());

    bool first = true;
    QVector<TreeItem *> items = treeModel->getRangeItems(index);
    foreach (TreeItem *current, items) {
        if (QStyle::State_MouseOver == (QStyle::State_MouseOver & opt.state)) {
            renderer.setProperty("hover", "line");
        } else {
            renderer.setProperty("hover", "");
        }
        if (!first) {
            renderer.setProperty("itemclass", "separator");
            widget->style()->unpolish(&renderer);
            widget->style()->polish(&renderer);

            const char s[] = {(const char)0xc2, (const char)0xbb, 0x00};// right double angle
            opt.text = QString::fromUtf8(s);
            opt.rect.setLeft(opt.rect.left() - opt.fontMetrics.width("  "));
            opt.rect.setWidth(opt.fontMetrics.width(opt.text+" ") + 0 + 2); // + padding + border

            renderer.setText(opt.text);
            renderer.resize(opt.rect.size());
            painter->save();
            painter->translate(opt.rect.topLeft());
            renderer.render(painter);
            painter->restore();

            opt.rect.setLeft(opt.rect.left() + opt.rect.width() - opt.fontMetrics.width(" "));
        } else {
            first = false;
        }

        renderer.setProperty("itemclass", current->getItemClass());
        if (widget->getCurrentHoveredItem() == current) {
            renderer.setProperty("hover", "exact");
        }
        widget->style()->unpolish(&renderer);
        widget->style()->polish(&renderer);

        opt.text = current->data(0).toString();
        opt.rect.setWidth(opt.fontMetrics.width(opt.text+"  ") + 0 + 2); // + padding + border
        renderer.setText(opt.text);
        renderer.resize(opt.rect.size());
        painter->save();
        painter->translate(opt.rect.topLeft());
        renderer.render(painter);
        painter->restore();

        opt.rect.setLeft(opt.rect.left() + opt.rect.width());
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
