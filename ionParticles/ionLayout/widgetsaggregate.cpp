/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "widgetsaggregate.h"
#include <ionCore/shared.h>

namespace IonLayout {


namespace Private {


WindowsTitleWatcher::WindowsTitleWatcher(QObject *parent) : QObject(parent)
{
}

bool WindowsTitleWatcher::eventFilter(QObject *obj, QEvent *event)
{
    if(QEvent::WindowTitleChange == event->type()) {
        QWidget *const window = qobject_cast<QWidget *>(obj);
        if(window) {
            emit titleChanged(window);
        }
    }
    return QObject::eventFilter(obj, event);
}



///////////


WidgetsSplitter::WidgetsSplitter(QWidget *parent)
{
    splitter = new QSplitter(parent);
}

void WidgetsSplitter::setOrientation(Qt::Orientation orientation)
{
    splitter->setOrientation(orientation);
}

void WidgetsSplitter::addWidget(QWidget *child)
{
    splitter->addWidget(child);
}

void WidgetsSplitter::insertWidget(int position, QWidget *child)
{
    splitter->insertWidget(position, child);
}

void WidgetsSplitter::remove(QWidget *child)
{
    child->close();
}

int WidgetsSplitter::indexOf(QWidget *child)
{
    return splitter->indexOf(child);
}

void WidgetsSplitter::focus(QWidget *child)
{
    child->setFocus();
}

void WidgetsSplitter::setSizes(QList<int> sizes)
{
    splitter->setSizes(sizes);
}

QWidget *WidgetsSplitter::getWidget()
{
    return splitter;
}


/////////////


WidgetsBoxed::WidgetsBoxed(QWidget *parent)
{
    widget = new QWidget(parent);
    layout = new QBoxLayout(QBoxLayout::LeftToRight);
    widget->setLayout(layout);
    layout->setMargin(0);
    layout->setSpacing(4);
}

void WidgetsBoxed::setOrientation(Qt::Orientation orientation)
{
    switch (orientation) {
        case Qt::Vertical:
            layout->setDirection(QBoxLayout::TopToBottom);
            break;
        case Qt::Horizontal:
            layout->setDirection(QBoxLayout::LeftToRight);
            break;
    }
}

void WidgetsBoxed::addWidget(QWidget *child)
{
    layout->addWidget(child);
}

void WidgetsBoxed::insertWidget(int position, QWidget *child)
{
    layout->insertWidget(position, child);
}

void WidgetsBoxed::remove(QWidget *child)
{
    child->close();
}

int WidgetsBoxed::indexOf(QWidget *child)
{
    return layout->indexOf(child);
}

void WidgetsBoxed::focus(QWidget *child)
{
    child->setFocus();
}

void WidgetsBoxed::setSizes(QList<int> sizes)
{
    int i = 0;
    foreach (int size, sizes) {
        layout->setStretch(i, size);
        i++;
    }
}

QWidget *WidgetsBoxed::getWidget()
{
    return widget;
}



////////////



void WidgetsTabbed::installTitleWatcher(QWidget *child)
{
    WindowsTitleWatcher *watcher = new WindowsTitleWatcher(child);
    connect(watcher, SIGNAL(titleChanged(QWidget *)), this, SLOT(onTitleChanged(QWidget *)));
    child->installEventFilter(watcher);
}

WidgetsTabbed::WidgetsTabbed(QWidget *parent, ZoneDefinition zoneDef)
{
    tabWidget = new QTabWidget(parent);
    tabWidget->setTabsClosable(zoneDef.childrenClosable);
    tabWidget->setMovable(true);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
}

void WidgetsTabbed::setOrientation(Qt::Orientation )
{
}

void WidgetsTabbed::addWidget(QWidget *child)
{
    tabWidget->addTab(child, child->windowTitle());
    installTitleWatcher(child);
}

void WidgetsTabbed::insertWidget(int position, QWidget *child)
{
    tabWidget->insertTab(position, child, child->windowTitle());
    installTitleWatcher(child);
}

void WidgetsTabbed::remove(QWidget *child)
{
    int idx = indexOf(child);
    if (-1 == idx) {
        throw std::runtime_error("widget must be already added to the panel");
    }
    closeAndRemoveTab(idx);
}

int WidgetsTabbed::indexOf(QWidget *child)
{
    return tabWidget->indexOf(child);
}

void WidgetsTabbed::focus(QWidget *child)
{
    int idx = indexOf(child);
    if (-1 == idx) {
        throw std::runtime_error("widget must be already added to the panel");
    }
    tabWidget->setCurrentIndex(idx);
    child->setFocus();
}

void WidgetsTabbed::setSizes(QList<int> )
{
}

QWidget *WidgetsTabbed::getWidget()
{
    return tabWidget;
}

void WidgetsTabbed::closeAndRemoveTab(int index)
{
    QWidget *w = tabWidget->widget(index);
    if (w->close()) {
        tabWidget->removeTab(index);
        delete w;
    }
    int idx = tabWidget->currentIndex();
    if (idx >= 0) {
        tabWidget->widget(idx)->setFocus();
    }
}

void WidgetsTabbed::onTitleChanged(QWidget *child)
{
    int idx = indexOf(child);
    if (-1 == idx) {
        throw std::runtime_error("widget must be already added to the panel");
    }
    tabWidget->setTabText(idx, child->windowTitle());
}

void WidgetsTabbed::onTabCloseRequested(int index)
{
    closeAndRemoveTab(index);
}



}
}
