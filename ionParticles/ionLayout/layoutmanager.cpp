/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "layoutmanager.h"
#include <ionCore/shared.h>

namespace IonLayout {


namespace Private {


LayoutZonesManager::LayoutZonesManager()
{
    root = boost::shared_ptr<ZoneNodeBranch>(new ZoneNodeRoot());
}
QWidget *LayoutZonesManager::getMainWidget()
{
    return root->getWidget();
}


ZoneNodeLeaf *LayoutZonesManager::getZone(QString path)
{
    return root->getZone(path)->getZoneLeaf();
}

void LayoutZonesManager::addZone(ZoneDefinition &zone)
{
   ZoneNodeBranch *parentNode = root->getZone(zone.parentPath)->getZoneAsBranch();
   Q_ASSERT(parentNode);
   ZoneNodeLeaf *leaf = new ZoneNodeLeaf(parentNode, zone);
   int pos;
   signed int index = -1;

   if ( zone.after.length() && ((pos = parentNode->indexOf(zone.after)) >= 0) ) {
       index = 1 + pos;
   } else if ( zone.before.length() && ((pos = parentNode->indexOf(zone.before)) >= 0) ) {
       index = pos;
   }
   parentNode->addSubZone(leaf, index);

   if (zone.hideIfEmpty) {
       leaf->getWidget()->hide();
   } else {
       leaf->show();
   }
}



///////////




LayoutManagerImpl::LayoutManagerImpl(QMainWindow *mainWindow) : zonesManager()
{
    QWidget *mainWidget = zonesManager.getMainWidget();
    mainWindow->setCentralWidget(mainWidget);
    mainWidget->setParent(mainWindow);
}

void LayoutManagerImpl::add(PanelWidget *panel)
{
    ZoneNodeLeaf *leaf = zonesManager.getZone(panel->getPanelZone());
    Q_ASSERT(leaf);
    leaf->add(panel);
    leaf->show();
}

void LayoutManagerImpl::remove(PanelWidget *panel)
{
    ZoneNodeLeaf *leaf = zonesManager.getZone(panel->getPanelZone());
    Q_ASSERT(leaf);
    leaf->remove(panel);
}

void LayoutManagerImpl::focus(PanelWidget *panel)
{
    ZoneNodeLeaf *leaf = zonesManager.getZone(panel->getPanelZone());
    Q_ASSERT(leaf);
    leaf->focus(panel);
    leaf->show();
}

void LayoutManagerImpl::addZone(ZoneDefinition &zone)
{
    zonesManager.addZone(zone);
}

}
}
