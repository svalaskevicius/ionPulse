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
//#include <QMap>
#include <stdexcept>
#include <QMainWindow>

#include <ionCore/shared.h>

#include "zonenode.h"

namespace IonLayout {

namespace Private {


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
