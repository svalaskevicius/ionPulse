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
    : ZoneNode(parent, zoneDef), splitter(new QSplitter()), subZones(ZoneList())
{
    splitter->setOrientation(zoneDef.orientation);
}

ZoneNodeBranch::~ZoneNodeBranch()
{
    delete splitter;
}


void ZoneNodeBranch::addSubZone(ZoneNode *child, int position)
{
    QString zoneName = child->getZoneName();
    subZones[zoneName] = child;
    splitter->insertWidget(position, child->getWidget());
    resizeChildren();
}

ZoneNodeBranch *ZoneNodeBranch::getZoneAsBranch()
{
    return this;
}

QWidget *ZoneNodeBranch::getWidget()
{
    return splitter;
}

void ZoneNodeBranch::show()
{
    splitter->show();
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
    QVector<int> vSizes(splitter->children().size());
    foreach (ZoneNode * const br, subZones) {
        int sizeWeight = br->getDefinition().sizeWeight;
        Q_ASSERT(sizeWeight); // not implemented support of sizeWeight = 0 (auto size adjusment)
        int i = splitter->indexOf(br->getWidget());
        vSizes[i] = sizeWeight;
    }
    splitter->setSizes(vSizes.toList());
}

int ZoneNodeBranch::indexOf(QWidget *child)
{
    return splitter->indexOf(child);
}




///////////





ZoneNodeLeaf::ZoneNodeLeaf(ZoneNodeBranch *parent, ZoneDefinition zoneDef)
    : QTabWidget(parent->getWidget()), ZoneNode(parent, zoneDef), parent(parent)
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

void ZoneNodeLeaf::closeAndRemoveTab( int index )
{
    QWidget *w = widget(index);
    if (w->close()) {
        removeTab(index);
        delete w;
    }
    int idx = currentIndex();
    if (idx >= 0) {
        widget(idx)->setFocus();
    }
}

void ZoneNodeLeaf::onTabCloseRequested ( int index )
{
    closeAndRemoveTab(index);
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

void LayoutManagerImpl::remove(PanelWidget *panel)
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
    leaf->closeAndRemoveTab(idx);
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

void LayoutManagerImpl::updatePanelTitle(IonLayout::PanelWidget *panel)
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
    uiTab->setTabText(idx, panel->getPanelTitle());
}

}
}
