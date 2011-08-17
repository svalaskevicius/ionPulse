#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include "panelwidget.h"
#include "mainwindow.h"

namespace IonHeart {

class LayoutManager
{
protected:
    MainWindow &mainWidget;
public:
    LayoutManager(MainWindow &mainWidget);
    void add(IPanelWidget *panel);
};
}

#endif // LAYOUTMANAGER_H
