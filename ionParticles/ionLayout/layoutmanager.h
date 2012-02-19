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

#include <QBoxLayout>

#include <boost/shared_ptr.hpp>


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
    virtual void insertWidget(int position, QWidget *) = 0;
    virtual int indexOf(QWidget *) = 0;
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
    virtual void insertWidget(int position, QWidget *widget) { splitter->insertWidget(position, widget); }
    virtual int indexOf(QWidget *child) { return splitter->indexOf(child); }
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
    virtual void insertWidget(int position, QWidget *child) { layout->insertWidget(position, child); }
    virtual int indexOf(QWidget *child) { return layout->indexOf(child); }
    virtual void setSizes(QList<int> sizes) { int i = 0; foreach (int size, sizes) { layout->setStretch(i, size); i++; } }
    virtual QWidget *getWidget() { return widget; }
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
    QTabWidget *tabWidget;
public:
    ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef);
    virtual ~ZoneNodeLeaf();
    virtual ZoneNodeLeaf *getZoneLeaf();
    virtual QTabWidget *getZoneContents();
    virtual ZoneNode *findSubZone(QStringList &path) throw();
    virtual ZoneNodeBranch *getZoneAsBranch();
    virtual QWidget *getWidget();
    virtual void show();
    void closeAndRemoveTab(int index);
protected slots:
    void onTabCloseRequested(int index);
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
public slots:
    void updatePanelTitle(IonLayout::PanelWidget *panel);
};
}
}

#endif // LAYOUTMANAGER_H
