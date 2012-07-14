/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef ZONENODE_H
#define ZONENODE_H

#include "layoutapi.h"

#include <ionCore/shared.h>

#include "widgetsaggregate.h"

namespace IonLayout {

namespace Private {


class ZoneNodeLeaf;
class ZoneNodeBranch;

class ZoneNode {
protected:
    ZoneNode *parent;
    ZoneDefinition  zoneDef;

    WidgetsAggregate *_createZoneWidget(ZoneDefinition::Type type, QWidget *parent, ZoneDefinition  zoneDef);
public:
    typedef QMap<QString, ZoneNode *> ZoneList;

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
    virtual ZoneList getSubZones() = 0;
    const ZoneDefinition & getDefinition() const;
};


class ZoneNodeBranch : public QObject, public ZoneNode
{
   Q_OBJECT
protected:
    ZoneList subZones;
    WidgetsAggregate *zoneImpl;
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
    virtual ZoneList getSubZones();
};

class ZoneNodeRoot : public ZoneNodeBranch {
public:
    ZoneNodeRoot()
        : ZoneNodeBranch(NULL, getEmptyZoneDef())
    {
    }
    static ZoneDefinition getEmptyZoneDef() {
        ZoneDefinition def;
        def.after = "";
        def.before = "";
        def.hideIfEmpty = false;
        def.name = "";
        def.orientation = Qt::Horizontal;
        def.sizeWeight = 1000;
        def.childrenClosable = false;
        def.subZonesContainerType = ZoneDefinition::Split;
        def.widgetsContainerType = ZoneDefinition::Split;
        return def;
    }
};

class ZoneNodeLeaf : public QObject, public ZoneNode
{
    Q_OBJECT
protected:
    ZoneNodeBranch *parent;
    WidgetsAggregate *zoneWidget;
public:
    ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef);
    virtual ~ZoneNodeLeaf();
    virtual ZoneNodeLeaf *getZoneLeaf();
    virtual ZoneNode *findSubZone(QStringList &path) throw();
    virtual ZoneNodeBranch *getZoneAsBranch();
    virtual QWidget *getWidget();
    virtual void show();
    void add(QWidget *panel);
    void remove(QWidget *panel);
    void focus(QWidget *panel);
    virtual ZoneList getSubZones();
};

}
}

#endif // ZONENODE_H
