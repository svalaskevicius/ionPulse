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
    ZoneDefinition  zoneDef;
public:
    ZoneNode(ZoneDefinition  zoneDef) : zoneDef(zoneDef) {}
    QString getZoneName() {return zoneDef.name;}
    virtual ZoneNode *findSubZone(QStringList &path) = 0;
    virtual ZoneNode *findZone(QString path) { if (!path.length()) {return this;} QStringList sl = path.split('/'); return findSubZone(sl); }
    virtual QTabWidget *getZoneContents() = 0;
    virtual ZoneNodeBranch *getZoneAsBranch() = 0;
};

class ZoneNodeBranch : public QSplitter, public ZoneNode
{
protected:
    typedef QMap<QString, ZoneNode *> ZoneList;
    ZoneList subZones;
public:
    ZoneNodeBranch(QWidget *parent) : QSplitter(parent), ZoneNode(ZoneDefinition()) {}
    ZoneNodeBranch(ZoneNodeBranch *parent, ZoneDefinition  zoneDef) : QSplitter(parent), ZoneNode(zoneDef)
    {
        setOrientation(zoneDef.orientation);
    }
    void addSubZone(ZoneNode *child) {subZones[child->getZoneName()] = child;}
    virtual ZoneNode *findSubZone(QStringList &path);
    virtual QTabWidget *getZoneContents() {
        QStringList empty;
        return findSubZone(empty)->getZoneContents();
    }
    virtual ZoneNodeBranch *getZoneAsBranch() {return this;}
};

class ZoneNodeLeaf : public QTabWidget, public ZoneNode
{
protected:
    ZoneNodeBranch *parent;
public:
    ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef) : QTabWidget(parent), ZoneNode(zoneDef), parent(parent) {}
    virtual QTabWidget *getZoneContents() {
        return this;
    }
    virtual ZoneNode *findSubZone(QStringList &path) {Q_ASSERT(path.size() == 0); return this;}
    virtual ZoneNodeBranch *getZoneAsBranch() {
        ZoneNodeBranch *br = new ZoneNodeBranch(parent, zoneDef);
        parent->addSubZone(br);
        parent->insertWidget(parent->indexOf(this), br);
        this->setParent(br);
        br->addWidget(this);
        return br;
    }
};

class LayoutZonesManager
{
protected:
    ZoneNodeBranch *root;
public:
    LayoutZonesManager(MainWindow &mainWidget);
    QTabWidget *getZone(QString path);
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
