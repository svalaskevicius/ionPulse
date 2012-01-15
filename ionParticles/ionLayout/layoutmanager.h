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

class ZoneNodeBranch : public QSplitter, public ZoneNode
{
   Q_OBJECT
private:
    bool childrenResized;
protected:
    virtual void resizeEvent ( QResizeEvent * event ) {
        QSplitter::resizeEvent(event);
        splitterResized();
    }
protected:
    typedef QMap<QString, ZoneNode *> ZoneList;
    ZoneList subZones;
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
protected:
    void splitterResized ( );
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

class ZoneNodeLeaf : public QTabWidget, public ZoneNode
{
    Q_OBJECT
protected:
    ZoneNodeBranch *parent;
public:
    ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef);
    virtual ~ZoneNodeLeaf();
    virtual ZoneNodeLeaf *getZoneLeaf();
    virtual QTabWidget *getZoneContents();
    virtual ZoneNode *findSubZone(QStringList &path) throw();
    virtual ZoneNodeBranch *getZoneAsBranch();
    virtual QWidget *getWidget();
    virtual void show();
    void closeAndRemoveTab( int index );
protected slots:
    void onTabCloseRequested ( int index );
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
    virtual void remove(PanelWidget *panel);
    virtual void focus(PanelWidget *panel);
    virtual void addZone(ZoneDefinition &zone);
};
}
}

#endif // LAYOUTMANAGER_H
