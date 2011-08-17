#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "panelwidget.h"
#include "mainwindow.h"
#include <QSplitter>
#include <QTabWidget>
#include <QString>
#include <QMap>


namespace IonHeart {

class ZoneNodeLeaf;

class ZoneNode {
protected:
    QString zoneName;
public:
    ZoneNode(QString name) : zoneName(name) {}
    QString getZoneName() {return zoneName;}
    virtual ZoneNodeLeaf *findZone(QStringList path) = 0;
};

class ZoneNodeBranch : public QSplitter, public ZoneNode
{
protected:
    typedef QMap<QString, ZoneNode *> ZoneList;
    ZoneList subZones;
public:
    ZoneNodeBranch(QWidget *parent) : QSplitter(parent), ZoneNode("") {}
    ZoneNodeBranch(ZoneNodeBranch *parent, QString name) : QSplitter(parent), ZoneNode(name) {}
    //virtual ZoneList getSubZones() {return subZones;}
    void addSubZone(ZoneNode *child) {subZones[child->getZoneName()] = child;}
    virtual ZoneNodeLeaf *findZone(QStringList path) {
        ZoneNode *next = subZones[path.front()];
        Q_ASSERT(next);
        path.pop_front();
        return next->findZone(path);
    }
};

class ZoneNodeLeaf : public QTabWidget, public ZoneNode
{
public:
    ZoneNodeLeaf(ZoneNodeBranch *parent, QString name) : QTabWidget(parent), ZoneNode(name) {}
    virtual ZoneNodeLeaf *findZone(QStringList path) {Q_ASSERT(path.size() == 0); return this;}
};

class LayoutZonesManager
{
protected:
    ZoneNodeBranch *root;
public:
    LayoutZonesManager(MainWindow &mainWidget);
    QTabWidget *getZone(QString path);
};



class LayoutManager
{
protected:
    LayoutZonesManager zonesManager;
public:
    LayoutManager(MainWindow &mainWidget);
    void add(IPanelWidget *panel);
};

}

#endif // LAYOUTMANAGER_H
