/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef WIDGETSAGGREGATE_H
#define WIDGETSAGGREGATE_H

#include "layoutapi.h"
#include <QSplitter>
#include <QTabWidget>
#include <QBoxLayout>
#include <stdexcept>
#include <QEvent>


#include <ionCore/shared.h>

namespace IonLayout {

namespace Private {

class WindowsTitleWatcher : public QObject
{
    Q_OBJECT
public:
    WindowsTitleWatcher(QObject *parent);
signals:
    void titleChanged(QWidget *);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class WidgetsAggregate  {
public:
    virtual ~WidgetsAggregate(){}
    virtual void setOrientation(Qt::Orientation) = 0;
    virtual void addWidget(QWidget *) = 0;
    virtual void insertWidget(int position, QWidget *) = 0;
    virtual void remove(QWidget *) = 0;
    virtual int indexOf(QWidget *) = 0;
    virtual void focus(QWidget *) = 0;
    virtual void setSizes(QList<int>) = 0;
    virtual QWidget *getWidget() = 0;
};


class WidgetsSplitter : public WidgetsAggregate
{
protected:
    QSplitter * splitter;
public:
    WidgetsSplitter(QWidget *parent);
    virtual void setOrientation(Qt::Orientation orientation);
    virtual void addWidget(QWidget *child);
    virtual void insertWidget(int position, QWidget *child);
    virtual void remove(QWidget *child);
    virtual int indexOf(QWidget *child);
    virtual void focus(QWidget *child);
    virtual void setSizes(QList<int> sizes);
    virtual QWidget *getWidget();
};

class WidgetsBoxed : public WidgetsAggregate
{
protected:
    QWidget *widget;
    QBoxLayout *layout;
public:
    WidgetsBoxed(QWidget *parent);
    virtual void setOrientation(Qt::Orientation orientation);
    virtual void addWidget(QWidget *child);
    virtual void insertWidget(int position, QWidget *child);
    virtual void remove(QWidget *child);
    virtual int indexOf(QWidget *child);
    virtual void focus(QWidget *child);
    virtual void setSizes(QList<int> sizes);
    virtual QWidget *getWidget();
};

class WidgetsTabbed : public QObject, public WidgetsAggregate
{
    Q_OBJECT
protected:
    QTabWidget *tabWidget;
    void installTitleWatcher(QWidget *child);
public:
    WidgetsTabbed(QWidget *parent, ZoneDefinition zoneDef);
    virtual void setOrientation(Qt::Orientation orientation);
    virtual void addWidget(QWidget *child);
    virtual void insertWidget(int position, QWidget *child);
    virtual void remove(QWidget *child);
    virtual int indexOf(QWidget *child);
    virtual void focus(QWidget *child);
    virtual void setSizes(QList<int> sizes);
    virtual QWidget *getWidget();
protected slots:
    void closeAndRemoveTab(int index);
    void onTitleChanged(QWidget *child);
    void onTabCloseRequested(int index);
    void onTabChanged(int index);
};

}
}

#endif // WIDGETSAGGREGATE_H
