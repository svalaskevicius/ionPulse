/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "zonenode.h"
#include <ionCore/shared.h>

namespace IonLayout {


namespace Private {

ZoneNode::ZoneNode(ZoneNode *parent, ZoneDefinition  zoneDef) : parent(parent), zoneDef(zoneDef)
{
}

WidgetsAggregate *ZoneNode::_createZoneWidget(ZoneDefinition::Type type, QWidget *parent, ZoneDefinition  zoneDef)
{
    switch (type) {
        case ZoneDefinition::Boxed:
            return new WidgetsBoxed(parent);
        case ZoneDefinition::Tabbed:
            return new WidgetsTabbed(parent, zoneDef);
        case ZoneDefinition::Split:
            return new WidgetsSplitter(parent);
        default:
            throw std::runtime_error("zone type is not set");
    }
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

void ZoneNodeLeaf::add(QWidget *panel)
{
    zoneWidget->addWidget(panel);
    zoneWidget->focus(panel);
}
void ZoneNodeLeaf::remove(QWidget *panel)
{
    zoneWidget->remove(panel);
}
void ZoneNodeLeaf::focus(QWidget *panel)
{
    zoneWidget->focus(panel);
}


}
}
