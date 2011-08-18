#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "panelwidget.h"
#include "mainwindow.h"
#include <QSplitter>
#include <QTabWidget>
#include <QString>
#include <QMap>
#include <stdexcept>


namespace IonHeart {

class ZoneNodeLeaf;
class ZoneNodeBranch;

class ZoneNode {
protected:
    ZoneNode *parent;
    ZoneDefinition  zoneDef;
public:
    ZoneNode(ZoneNode *parent, ZoneDefinition  zoneDef) : parent(parent), zoneDef(zoneDef) {}
    QString getZoneName() {return zoneDef.name;}
    virtual ZoneNode *findSubZone(QStringList &path) throw() = 0;
    virtual ZoneNode *getSubZone(QStringList &path) throw(std::runtime_error);
    virtual ZoneNodeLeaf *getZoneLeaf() = 0;
    virtual ZoneNode *getZone(QString path);
    virtual ZoneNodeBranch *getZoneAsBranch() = 0;
    virtual QWidget *getWidget() = 0;
    virtual void show() {
        if (parent) {
            parent->show();
        }
    }
   // const ZoneDefinition getDefinition() const {return zoneDef;}
};

class ZoneNodeBranch : public QSplitter, public ZoneNode
{
protected:
    typedef QMap<QString, ZoneNode *> ZoneList;
    ZoneList subZones;
public:
    ZoneNodeBranch(QWidget *parent) : QSplitter(parent), ZoneNode(NULL, ZoneDefinition()) {}
    ZoneNodeBranch(ZoneNodeBranch *parent, ZoneDefinition  zoneDef) : QSplitter(parent), ZoneNode(parent, zoneDef)
    {
        setOrientation(zoneDef.orientation);
    }
    void addSubZone(ZoneNode *child) {subZones[child->getZoneName()] = child;}
    virtual ZoneNode *findSubZone(QStringList &path) throw();
    virtual ZoneNodeLeaf *getZoneLeaf();
    virtual ZoneNodeBranch *getZoneAsBranch() {return this;}
    virtual QWidget *getWidget() {return this;}
    virtual void show()
    {
        QSplitter::show();
        ZoneNode::show();
    }
};

class ZoneNodeLeaf : public QTabWidget, public ZoneNode
{
protected:
    ZoneNodeBranch *parent;
public:
    ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef) : QTabWidget(parent), ZoneNode(parent, zoneDef), parent(parent) {}
    virtual ZoneNodeLeaf *getZoneLeaf() {
        return this;
    }
    virtual QTabWidget *getZoneContents() {
        return this;
    }
    virtual ZoneNode *findSubZone(QStringList &path) throw() {Q_ASSERT(path.size() == 0); return this;}
    virtual ZoneNodeBranch *getZoneAsBranch();
    virtual QWidget *getWidget() {return this;}
    virtual void show()
    {
        QTabWidget::show();
        ZoneNode::show();
    }
};

class LayoutZonesManager
{
protected:
    ZoneNodeBranch *root;
public:
    LayoutZonesManager(MainWindow &mainWidget);
    ZoneNodeLeaf *getZone(QString path);
    void addZone(ZoneDefinition &zone);
};



class LayoutManager
{
protected:
    LayoutZonesManager zonesManager;
public:
    LayoutManager(MainWindow &mainWidget);
    void add(IPanelWidget *panel);
    void addZone(ZoneDefinition &zone);
};

}

#endif // LAYOUTMANAGER_H
