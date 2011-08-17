#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>

namespace IonHeart {

class IPanelWidget {
public:
    virtual QWidget *getWidget() = 0;
    virtual QString getPanelTitle() = 0;
    virtual QString getPanelZone() = 0;
};

}

#endif // PANELWIDGET_H
