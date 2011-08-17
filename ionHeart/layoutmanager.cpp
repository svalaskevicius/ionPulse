#include "layoutmanager.h"


namespace IonHeart {

LayoutManager::LayoutManager(MainWindow &mainWidget) : mainWidget(mainWidget)
{
}

void LayoutManager::add(IPanelWidget *panel) {
    mainWidget.setCentralWidget(panel->getWidget());
}


}
