/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "layoutmanager.h"


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
    : QSplitter(parent), ZoneNode(parent, zoneDef), childrenResized(false)
{
    setOrientation(zoneDef.orientation);
}

ZoneNodeBranch::~ZoneNodeBranch()
{
}


void ZoneNodeBranch::addSubZone(ZoneNode *child, int position)
{
    QString zoneName = child->getZoneName();
    subZones[zoneName] = child;
    this->insertWidget(position, child->getWidget());
}

ZoneNodeBranch *ZoneNodeBranch::getZoneAsBranch()
{
    return this;
}

QWidget *ZoneNodeBranch::getWidget()
{
    return this;
}

void ZoneNodeBranch::show()
{
    QSplitter::show();
    ZoneNode::show();
}

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

void ZoneNodeBranch::splitterResized ( ) {
    if (!childrenResized) {
        resizeChildren();
        childrenResized = true;
    }
}
void ZoneNodeBranch::resizeChildren() {
    QVector<int> vSizes(this->children().size());
    foreach (ZoneNode * const br, subZones) {
        int sizeWeight = br->getDefinition().sizeWeight;
        Q_ASSERT(sizeWeight); // not implemented support of sizeWeight = 0 (auto size adjusment)
        int i = this->indexOf(br->getWidget());
        printf("nfo: name: %s, size: %d, i: %d\n", br->getDefinition().name.toAscii().constData(), sizeWeight, i);
        vSizes[i] = sizeWeight;
    }
    this->setSizes(vSizes.toList());
}






///////////





ZoneNodeLeaf::ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef)
    : QTabWidget(parent), ZoneNode(parent, zoneDef), parent(parent)
{
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
}
ZoneNodeLeaf::~ZoneNodeLeaf()
{
}

ZoneNodeLeaf *ZoneNodeLeaf::getZoneLeaf()
{
    return this;
}
QTabWidget *ZoneNodeLeaf::getZoneContents()
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
    return this;
}
void ZoneNodeLeaf::show()
{
    QTabWidget::show();
    ZoneNode::show();
}
ZoneNodeBranch *ZoneNodeLeaf::getZoneAsBranch() {
    ZoneNodeBranch *br = new ZoneNodeBranch(parent, zoneDef);
    parent->addSubZone(br, parent->indexOf(this));
    br->addSubZone(this);
    parent = br;
    if (this->isHidden()) {
        br->getWidget()->hide();
    }
    return br;
}
void ZoneNodeLeaf::onTabCloseRequested ( int index )
{
    QWidget *w = widget(index);
    if (w->close()) {
        removeTab(index);
        delete w;
    }

}





///////////




LayoutZonesManager::LayoutZonesManager()
{
    root = new ZoneNodeRoot();
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
   ZoneNode *pos;
   signed int index = -1;

   if ( zone.after.length() && (pos = parentNode->findSubZone(QStringList() << zone.after)) ) {
       index = 1 + parentNode->indexOf(pos->getWidget());
   } else if ( zone.before.length() && (pos = parentNode->findSubZone(QStringList() << zone.before)) ) {
       index = parentNode->indexOf(pos->getWidget());
   }
   parentNode->addSubZone(leaf, index);

   leaf->setTabsClosable(zone.childrenClosable);
   leaf->setMovable(true);

   if (zone.hideIfEmpty) {
       leaf->hide();
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
    QTabWidget *uiTab = leaf->getZoneContents();
    Q_ASSERT(uiTab);
    int idx = uiTab->addTab(
        panel->getWidget(),
        panel->getPanelTitle()
    );
    uiTab->setCurrentIndex(idx);
    uiTab->widget(idx)->setFocus();
    leaf->show();
}

void LayoutManagerImpl::focus(PanelWidget *panel)
{
    ZoneNodeLeaf *leaf = zonesManager.getZone(panel->getPanelZone());
    Q_ASSERT(leaf);
    QTabWidget *uiTab = leaf->getZoneContents();
    Q_ASSERT(uiTab);
    int idx = uiTab->indexOf(
        panel->getWidget()
    );
    if (-1 == idx) {
        throw std::runtime_error("widget must be already added to the panel");
    }
    uiTab->setCurrentIndex(idx);
    uiTab->widget(idx)->setFocus();
    leaf->show();
}

void LayoutManagerImpl::addZone(ZoneDefinition &zone)
{
    zonesManager.addZone(zone);
}

}
}
