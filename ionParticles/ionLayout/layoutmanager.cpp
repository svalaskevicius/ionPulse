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

ZoneNode::ZoneNode(ZoneNode *parent, ZoneDefinition  zoneDef) : parent(parent), zoneDef(zoneDef)
{
}

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

QString ZoneNode::getZoneName()
{
    return zoneDef.name;
}

void ZoneNode::show()
{
    if (parent) {
        parent->show();
    }
}

const ZoneDefinition & ZoneNode::getDefinition() const
{
    return zoneDef;
}




///////////


ZoneNodeBranch::ZoneNodeBranch(ZoneNodeBranch *parent, ZoneDefinition  zoneDef)
    : ZoneNode(parent, zoneDef), subZones(ZoneList())
{
    QWidget *parentWidget = NULL;
    if (parent) {
        parentWidget = parent->getWidget();
    }
    zoneImpl = _createZoneWidget(zoneDef.subZonesContainerType, parentWidget, zoneDef);
    zoneImpl->setOrientation(zoneDef.orientation);
}

ZoneNodeBranch::~ZoneNodeBranch()
{
    delete zoneImpl;
    qDeleteAll(subZones);
}


void ZoneNodeBranch::addSubZone(ZoneNode *child, int position)
{
    QString zoneName = child->getZoneName();
    subZones[zoneName] = child;
    zoneImpl->insertWidget(position, child->getWidget());
    resizeChildren();
}

ZoneNodeBranch *ZoneNodeBranch::getZoneAsBranch()
{
    return this;
}

QWidget *ZoneNodeBranch::getWidget()
{
    return zoneImpl->getWidget();
}

void ZoneNodeBranch::show()
{
    zoneImpl->getWidget()->show();
    ZoneNode::show();
}

ZoneNode *ZoneNodeBranch::findSubZone(QStringList &path) throw()
{
    if (!path.length()) {
        path << getZoneName();
    }
    ZoneList::iterator subz = subZones.find(path.front());
    if (subZones.end() == subz) {
        return NULL;
    }
    path.pop_front();
    return subz.value()->findSubZone(path);
}

ZoneNodeLeaf *ZoneNodeBranch::getZoneLeaf() {
    QStringList empty;
    return getSubZone(empty)->getZoneLeaf();
}

void ZoneNodeBranch::resizeChildren() {
    QVector<int> vSizes(zoneImpl->getWidget()->children().size());
    foreach (ZoneNode * const br, subZones) {
        int sizeWeight = br->getDefinition().sizeWeight;
        Q_ASSERT(sizeWeight); // not implemented support of sizeWeight = 0 (auto size adjusment)
        int i = zoneImpl->indexOf(br->getWidget());
        vSizes[i] = sizeWeight;
    }
    zoneImpl->setSizes(vSizes.toList());
}

int ZoneNodeBranch::indexOf(QWidget *child)
{
    return zoneImpl->indexOf(child);
}


int ZoneNodeBranch::indexOf(QString childName)
{
    if (subZones.contains(childName)) {
        return indexOf(subZones[childName]->getWidget());
    }
    return -1;
}


///////////





ZoneNodeLeaf::ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef)
    : ZoneNode(parent, zoneDef), parent(parent)
{
    zoneWidget = _createZoneWidget(zoneDef.widgetsContainerType, parent->getWidget(), zoneDef);
}
ZoneNodeLeaf::~ZoneNodeLeaf()
{
    delete zoneWidget;
}

ZoneNodeLeaf *ZoneNodeLeaf::getZoneLeaf()
{
    return this;
}
ZoneNode *ZoneNodeLeaf::findSubZone(QStringList &path) throw()
{
    Q_ASSERT(path.size() == 0);
    return this;
}
QWidget *ZoneNodeLeaf::getWidget()
{
    return zoneWidget->getWidget();
}
void ZoneNodeLeaf::show()
{
    zoneWidget->getWidget()->show();
    ZoneNode::show();
}
ZoneNodeBranch *ZoneNodeLeaf::getZoneAsBranch() {
    ZoneNodeBranch *br = new ZoneNodeBranch(parent, zoneDef);
    parent->addSubZone(br, parent->indexOf(zoneWidget->getWidget()));
    br->addSubZone(this);
    parent = br;
    if (zoneWidget->getWidget()->isHidden()) {
        br->getWidget()->hide();
    }
    return br;
}

void ZoneNodeLeaf::add(IonLayout::PanelWidget *panel)
{
    zoneWidget->addWidget(panel->getWidget());
    zoneWidget->focus(panel->getWidget());
}
void ZoneNodeLeaf::remove(IonLayout::PanelWidget *panel)
{
    zoneWidget->remove(panel->getWidget());
}
void ZoneNodeLeaf::focus(IonLayout::PanelWidget *panel)
{
    zoneWidget->focus(panel->getWidget());
}



///////////




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
