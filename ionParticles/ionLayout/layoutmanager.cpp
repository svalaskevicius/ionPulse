/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "layoutmanager.h"


namespace IonLayout {


namespace Private {

ZoneNode *ZoneNode::getSubZone(QStringList &path) throw(std::runtime_error)
{
    ZoneNode *ret = findSubZone(path);
    if (!ret) {
        throw std::runtime_error(("zone '"+path.join(".")+"' not found").toStdString());
    }
    return ret;
}

ZoneNode *ZoneNode::getZone(QString path)
{
    if (!path.length()) {
        return this;
    }
    QStringList sl = path.split('/');
    return getSubZone(sl);
}




///////////




ZoneNode *ZoneNodeBranch::findSubZone(QStringList &path) throw()
{
    if (!path.length()) {
        path << getZoneName();
    }
    ZoneNode *next = subZones[path.front()];
    if (!next) {
        return NULL;
    }
    path.pop_front();
    return next->findSubZone(path);
}

ZoneNodeLeaf *ZoneNodeBranch::getZoneLeaf() {
    QStringList empty;
    return getSubZone(empty)->getZoneLeaf();
}

void ZoneNodeBranch::resizeEvent ( QResizeEvent * event ) {
    if (!childrenResized) {
        resizeChildren();
        childrenResized = true;
    }
    //uiSplitter.resizeEvent(event);
}
void ZoneNodeBranch::resizeChildren() {
    QVector<int> vSizes(uiSplitter.children().size());
    foreach (ZoneNode * const br, subZones) {
        int sizeWeight = br->getDefinition().sizeWeight;
        Q_ASSERT(sizeWeight); // not implemented support of sizeWeight = 0 (auto size adjusment)
        int i = uiSplitter.indexOf(br->getWidget());
        printf("nfo: name: %s, size: %d, i: %d\n", br->getDefinition().name.toAscii().constData(), sizeWeight, i);
        vSizes[i] = sizeWeight;
    }
    uiSplitter.setSizes(vSizes.toList());
}





///////////





ZoneNodeBranch *ZoneNodeLeaf::getZoneAsBranch() {
    ZoneNodeBranch *br = new ZoneNodeBranch(parent, zoneDef);
    parent->addSubZone(br);
//    parent->insertWidget(parent->indexOf(this), br);
    br->addSubZone(this);
   // uiTab.setParent(br->getWidget());
     parent = br;
//    br->addWidget(this);
    if (uiTab.isHidden()) {
//        br->hide();
    }
    return br;
}




///////////




LayoutZonesManager::LayoutZonesManager()
{
    root = new ZoneNodeBranch();
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
   ZoneNodeBranch *node = root->getZone(zone.parentPath)->getZoneAsBranch();
   Q_ASSERT(node);
   ZoneNodeLeaf *leaf = new ZoneNodeLeaf(node, zone);
   node->addSubZone(leaf);
   ZoneNode *pos;
   signed int index = -1;

   if ( zone.after.length() && (pos = node->findSubZone(QStringList() << zone.after)) ) {
//       index = 1+node->indexOf(pos->getWidget());
   } else if ( zone.before.length() && (pos = node->findSubZone(QStringList() << zone.before)) ) {
//       index = node->indexOf(pos->getWidget());
   }
   if (index > -1) {
//       leaf->setParent(0);
//       node->insertWidget(index, leaf);
   }

   if (zone.hideIfEmpty) {
//       leaf->hide();
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
    ZoneNodeLeaf *z = zonesManager.getZone(panel->getPanelZone());
    Q_ASSERT(z);
//    int idx = z->addTab(
//        panel->getWidget(),
//        panel->getPanelTitle()
//    );
//    z->setCurrentIndex(idx);
//    z->widget(idx)->setFocus();
    z->show();
}

void LayoutManagerImpl::addZone(ZoneDefinition &zone)
{
    zonesManager.addZone(zone);
}

}
}
