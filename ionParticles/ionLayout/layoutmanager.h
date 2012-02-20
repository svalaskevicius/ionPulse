/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "layoutapi.h"
#include <QSplitter>
#include <QTabWidget>
#include <QString>
#include <QMap>
#include <stdexcept>
#include <QMainWindow>
#include <QEvent>

#include <QBoxLayout>

#include <boost/shared_ptr.hpp>

#include <ionCore/shared.h>

namespace IonLayout {

namespace Private {


class ZoneNodeLeaf;
class ZoneNodeBranch;

class ZoneNode {
protected:
    ZoneNode *parent;
    ZoneDefinition  zoneDef;
public:
    ZoneNode(ZoneNode *parent, ZoneDefinition  zoneDef);
    virtual ~ZoneNode(){}
    QString getZoneName();
    virtual ZoneNode *findSubZone(QStringList &path) throw() = 0;
    virtual ZoneNode *getSubZone(QStringList &path) throw(std::runtime_error);
    virtual ZoneNodeLeaf *getZoneLeaf() = 0;
    virtual ZoneNode *getZone(QString path);
    virtual ZoneNodeBranch *getZoneAsBranch() = 0;
    virtual QWidget *getWidget() = 0;
    virtual void show();
    const ZoneDefinition & getDefinition() const;
};

class ZoneWidget  {
public:
    virtual void setOrientation(Qt::Orientation) = 0;
    virtual void addWidget(QWidget *) = 0;
    virtual void insertWidget(int position, QWidget *) = 0;
    virtual void remove(QWidget *) = 0;
    virtual int indexOf(QWidget *) = 0;
    virtual int focus(QWidget *) = 0;
    virtual void setSizes(QList<int>) = 0;
    virtual QWidget *getWidget() = 0;
};

class ZoneWidgetSplitter : public ZoneWidget
{
protected:
    QSplitter *splitter;
public:
    ZoneWidgetSplitter() {splitter = new QSplitter();}
    virtual void setOrientation(Qt::Orientation orientation) { splitter->setOrientation(orientation); }
    virtual void addWidget(QWidget *child) { splitter->addWidget(child); }
    virtual void insertWidget(int position, QWidget *child) { splitter->insertWidget(position, child); }
    virtual void remove(QWidget *child) { child->close(); }
    virtual int indexOf(QWidget *child) { return splitter->indexOf(child); }
    virtual int focus(QWidget *child) { child->setFocus(); }
    virtual void setSizes(QList<int> sizes) {splitter->setSizes(sizes);}
    virtual QWidget *getWidget() { return splitter; }
};

class ZoneWidgetBoxed : public ZoneWidget
{
protected:
    QWidget *widget;
    QBoxLayout *layout;
public:
    ZoneWidgetBoxed() {
        widget = new QWidget();layout = new QBoxLayout(QBoxLayout::LeftToRight);widget->setLayout(layout);
        layout->setMargin(0);
    }
    virtual void setOrientation(Qt::Orientation orientation)
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
    virtual void addWidget(QWidget *child) { layout->addWidget(child); }
    virtual void insertWidget(int position, QWidget *child) { layout->insertWidget(position, child); }
    virtual void remove(QWidget *child) { child->close(); }
    virtual int indexOf(QWidget *child) { return layout->indexOf(child); }
    virtual int focus(QWidget *child) { child->setFocus(); }
    virtual void setSizes(QList<int> sizes) { int i = 0; foreach (int size, sizes) { layout->setStretch(i, size); i++; } }
    virtual QWidget *getWidget() { return widget; }
};

class WindowsTitleWatcher : public QObject
{
    Q_OBJECT
public:
    WindowsTitleWatcher(QObject *parent) : QObject(parent) { }

signals:
    void titleChanged(QWidget *);

protected:
    bool eventFilter(QObject *obj, QEvent *event){
        if(QEvent::WindowTitleChange == event->type()) {
            QWidget *const window = qobject_cast<QWidget *>(obj);
            if(window) {
                emit titleChanged(window);
            }
        }
        return QObject::eventFilter(obj, event);
    }
};

class ZoneWidgetTabbed : public QObject, public ZoneWidget
{
    Q_OBJECT
protected:
    QTabWidget *tabWidget;
    void installTitleWatcher(QWidget *child) {
        WindowsTitleWatcher *watcher = new WindowsTitleWatcher(child);
        connect(watcher, SIGNAL(titleChanged(QWidget *)), this, SLOT(onTitleChanged(QWidget *)));
        child->installEventFilter(watcher);
    }
public:
    ZoneWidgetTabbed(ZoneDefinition zoneDef) {
        tabWidget = new QTabWidget();
        tabWidget->setTabsClosable(zoneDef.childrenClosable);
        tabWidget->setMovable(true);
        connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
    }
    virtual void setOrientation(Qt::Orientation orientation) { }
    virtual void addWidget(QWidget *child) { tabWidget->addTab(child, child->windowTitle()); installTitleWatcher(child); }
    virtual void insertWidget(int position, QWidget *child) { tabWidget->insertTab(position, child, child->windowTitle()); installTitleWatcher(child); }
    virtual void remove(QWidget *child) {
        int idx = indexOf(child);
        if (-1 == idx) {
            throw std::runtime_error("widget must be already added to the panel");
        }
        closeAndRemoveTab(idx);
    }
    virtual int indexOf(QWidget *child) { return tabWidget->indexOf(child); }
    virtual int focus(QWidget *child) {
        int idx = indexOf(child);
        if (-1 == idx) {
            throw std::runtime_error("widget must be already added to the panel");
        }
        tabWidget->setCurrentIndex(idx);
        child->setFocus();
    }
    virtual void setSizes(QList<int> sizes) { }
    virtual QWidget *getWidget() { return tabWidget; }
protected slots:
    void closeAndRemoveTab(int index)
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
    void onTitleChanged(QWidget *child) {
        int idx = indexOf(child);
        if (-1 == idx) {
            throw std::runtime_error("widget must be already added to the panel");
        }
        tabWidget->setTabText(idx, child->windowTitle());
    }
    void onTabCloseRequested(int index)
    {
        closeAndRemoveTab(index);
    }

};

class ZoneNodeBranch : public QObject, public ZoneNode
{
   Q_OBJECT
protected:
    typedef QMap<QString, ZoneNode *> ZoneList;
    ZoneList subZones;
    ZoneWidget *zoneImpl;

    ZoneWidget *_createZoneWidget(ZoneDefinition  zoneDef) {
        switch (zoneDef.type) {
            case ZoneDefinition::Boxed:
                return new ZoneWidgetBoxed();
            case ZoneDefinition::Split:
            default:
                return new ZoneWidgetSplitter();
        }
    }
public:
    ZoneNodeBranch(ZoneNodeBranch *parent, ZoneDefinition  zoneDef);
    virtual ~ZoneNodeBranch();
    void addSubZone(ZoneNode *child, int position = -1);
    virtual ZoneNode *findSubZone(QStringList &path) throw();
    virtual ZoneNodeLeaf *getZoneLeaf();
    virtual ZoneNodeBranch *getZoneAsBranch();
    virtual QWidget *getWidget();
    virtual void show();
    void resizeChildren();
    int indexOf(QWidget *);
    int indexOf(QString);
};

class ZoneNodeRoot : public ZoneNodeBranch {
public:
    ZoneNodeRoot()
        : ZoneNodeBranch(NULL, getEmptyZoneDef())
    {
    }
protected:
    static ZoneDefinition getEmptyZoneDef() {
        ZoneDefinition def;
        def.after = "";
        def.before = "";
        def.hideIfEmpty = false;
        def.name = "";
        def.orientation = Qt::Horizontal;
        def.sizeWeight = 1000;
        return def;
    }
};

class ZoneNodeLeaf : public QObject, public ZoneNode
{
    Q_OBJECT
protected:
    ZoneNodeBranch *parent;
    ZoneWidget *zoneWidget;
public:
    ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef);
    virtual ~ZoneNodeLeaf();
    virtual ZoneNodeLeaf *getZoneLeaf();
    virtual ZoneNode *findSubZone(QStringList &path) throw();
    virtual ZoneNodeBranch *getZoneAsBranch();
    virtual QWidget *getWidget();
    virtual void show();
    void add(IonLayout::PanelWidget *panel);
    void remove(IonLayout::PanelWidget *panel);
    void focus(IonLayout::PanelWidget *panel);
};

class LayoutZonesManager
{
protected:
    boost::shared_ptr<ZoneNodeBranch> root;
public:
    LayoutZonesManager();
    ZoneNodeLeaf *getZone(QString path);
    void addZone(ZoneDefinition &zone);
    QWidget *getMainWidget();
};


class LayoutManagerImpl : public LayoutManager
{
    Q_OBJECT
protected:
    LayoutZonesManager zonesManager;
public:
    LayoutManagerImpl(QMainWindow *mainWindow);
    virtual void add(PanelWidget *panel);
    virtual void remove(PanelWidget *panel);
    virtual void focus(PanelWidget *panel);
    virtual void addZone(ZoneDefinition &zone);
};
}
}

#endif // LAYOUTMANAGER_H
