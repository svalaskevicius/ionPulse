/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "layoutapi.h"
//#include <QString>
#include <QMap>
#include <stdexcept>
#include <QMainWindow>

#include <ionCore/shared.h>

#include "zonenode.h"

namespace IonLayout {

namespace Private {


class LayoutZonesManager
{
protected:
    QSharedPointer<ZoneNodeBranch> root;
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
    QMap<QWidget *, QString> widgetZoneMap;
public:
    LayoutManagerImpl(QMainWindow *mainWindow);
public slots:
    virtual void add(QString zonePath, QWidget *widget);
    virtual void remove(QWidget *widget);
    virtual void focus(QWidget *widget);
    virtual void addZone(ZoneDefinition &zone);
};
}
}

#endif // LAYOUTMANAGER_H
