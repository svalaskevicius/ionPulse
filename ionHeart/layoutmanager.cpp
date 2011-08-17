#include "layoutmanager.h"


namespace IonHeart {

LayoutZonesManager::LayoutZonesManager(MainWindow &mainWidget)
{
    root = new ZoneNodeBranch(&mainWidget);
    root->addSubZone(new ZoneNodeLeaf(root, "content"));
    mainWidget.setCentralWidget(root);
}

QTabWidget *LayoutZonesManager::getZone(QString path)
{
    return dynamic_cast<QTabWidget *>(root->findZone(path.split('/')));
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


}
