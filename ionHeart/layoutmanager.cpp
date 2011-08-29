#include "layoutmanager.h"


namespace IonHeart {


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
    QSplitter::resizeEvent(event);
}
void ZoneNodeBranch::resizeChildren() {
    QVector<int> vSizes(this->children().size());
    foreach (ZoneNode * const br, subZones) {
        int sw = br->getDefinition().sizeWeight;
        Q_ASSERT(sw); // not implemented support of sw = 0 (auto size adjusment)
        int i = indexOf(br->getWidget());
        printf("nfo: name: %s, size: %d, i: %d\n", br->getDefinition().name.toAscii().constData(), sw, i);
        vSizes[i] = sw;
    }
    this->setSizes(vSizes.toList());
}


///////////


ZoneNodeBranch *ZoneNodeLeaf::getZoneAsBranch() {
    ZoneNodeBranch *br = new ZoneNodeBranch(parent, zoneDef);
    parent->addSubZone(br);
    parent->insertWidget(parent->indexOf(this), br);
    br->addSubZone(this);
    this->setParent(br);
    ZoneNode::parent = br;
    br->addWidget(this);
    if (this->isHidden()) {
        br->hide();
    }
    return br;
}

LayoutZonesManager::LayoutZonesManager(MainWindow &mainWidget)
{
    root = new ZoneNodeBranch(&mainWidget);
    mainWidget.setCentralWidget(root);
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
       index = 1+node->indexOf(pos->getWidget());
   } else if ( zone.before.length() && (pos = node->findSubZone(QStringList() << zone.before)) ) {
       index = node->indexOf(pos->getWidget());
   }
   if (index > -1) {
       leaf->setParent(0);
       node->insertWidget(index, leaf);
   }

   if (zone.hideIfEmpty) {
       leaf->hide();
   } else {
       leaf->show();
   }
}

///////////

LayoutManager::LayoutManager(MainWindow &mainWidget) : zonesManager(mainWidget)
{
}

void LayoutManager::add(IPanelWidget *panel)
{
    ZoneNodeLeaf *z = zonesManager.getZone(panel->getPanelZone());
    Q_ASSERT(z);
    z->addTab(
        panel->getWidget(),
        panel->getPanelTitle()
    );
    z->show();
}

void LayoutManager::addZone(ZoneDefinition &zone)
{
    zonesManager.addZone(zone);
}


}
