#include "layoutmanager.h"


namespace IonHeart {


ZoneNode *ZoneNodeBranch::findSubZone(QStringList &path)
{
    if (!path.length()) {
        path << getZoneName();
    }
    ZoneNode *next = subZones[path.front()];
    if (!next) {
        throw std::runtime_error(("zone element '"+path.front()+"' not found").toStdString());
    }
    path.pop_front();
    return next->findSubZone(path);
}


LayoutZonesManager::LayoutZonesManager(MainWindow &mainWidget)
{
    root = new ZoneNodeBranch(&mainWidget);
    mainWidget.setCentralWidget(root);
}

QTabWidget *LayoutZonesManager::getZone(QString path)
{
    return root->findZone(path)->getZoneContents();
}


void LayoutZonesManager::addZone(ZoneDefinition &zone)
{
    printf("AA: %s\n%s\n", zone.parentPath.toAscii().constData(), zone.name.toAscii().constData());
   ZoneNodeBranch *node = root->findZone(zone.parentPath)->getZoneAsBranch();
   Q_ASSERT(node);
   node->addSubZone(new ZoneNodeLeaf(node, zone));
}

///
LayoutManager::LayoutManager(MainWindow &mainWidget) : zonesManager(mainWidget)
{
}

void LayoutManager::add(IPanelWidget *panel)
{
    QTabWidget *z = zonesManager.getZone(panel->getPanelZone());
    Q_ASSERT(z);
    z->addTab(
        panel->getWidget(),
        panel->getPanelTitle()
    );
}

void LayoutManager::addZone(ZoneDefinition &zone)
{
    zonesManager.addZone(zone);
}


}
