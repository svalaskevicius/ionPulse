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


namespace IonLayout {

namespace Private {

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
    const ZoneDefinition & getDefinition() const {return zoneDef;}
};

class ZoneNodeBranch : public ZoneNode
{
private:
    QSplitter uiSplitter;
    bool childrenResized;
protected:
    typedef QMap<QString, ZoneNode *> ZoneList;
    ZoneList subZones;
    virtual void resizeEvent ( QResizeEvent * event );
public:
    ZoneNodeBranch() : ZoneNode(NULL, ZoneDefinition()), uiSplitter(NULL), childrenResized(false) {}
    ZoneNodeBranch(ZoneNodeBranch *parent, ZoneDefinition  zoneDef) : ZoneNode(parent, zoneDef), uiSplitter(), childrenResized(false)
    {
        uiSplitter.setOrientation(zoneDef.orientation);
    }
    void addSubZone(ZoneNode *child) {subZones[child->getZoneName()] = child;}
    virtual ZoneNode *findSubZone(QStringList &path) throw();
    virtual ZoneNodeLeaf *getZoneLeaf();
    virtual ZoneNodeBranch *getZoneAsBranch() {return this;}
    virtual QWidget *getWidget() {return &uiSplitter;}
    virtual void show()
    {
        uiSplitter.show();
        ZoneNode::show();
    }
    void resizeChildren();
};

class ZoneNodeLeaf : public ZoneNode
{
protected:
    QTabWidget uiTab;
    ZoneNodeBranch *parent;
public:
    ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef) : ZoneNode(parent, zoneDef), uiTab(), parent(parent) {}
    virtual ZoneNodeLeaf *getZoneLeaf() {
        return this;
    }
    virtual QTabWidget *getZoneContents() {
        return &uiTab;
    }
    virtual ZoneNode *findSubZone(QStringList &path) throw() {Q_ASSERT(path.size() == 0); return this;}
    virtual ZoneNodeBranch *getZoneAsBranch();
    virtual QWidget *getWidget() {return &uiTab;}
    virtual void show()
    {
        uiTab.show();
        ZoneNode::show();
    }
};

class LayoutZonesManager
{
protected:
    ZoneNodeBranch *root;
public:
    LayoutZonesManager();
    ZoneNodeLeaf *getZone(QString path);
    void addZone(ZoneDefinition &zone);
    QWidget *getMainWidget();
};


class LayoutManagerImpl : public LayoutManager
{
protected:
    LayoutZonesManager zonesManager;
public:
    LayoutManagerImpl(QMainWindow *mainWindow);
    virtual void add(PanelWidget *panel);
    virtual void addZone(ZoneDefinition &zone);
};
}
}

#endif // LAYOUTMANAGER_H
