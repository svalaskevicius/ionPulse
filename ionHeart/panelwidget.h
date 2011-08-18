#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>

namespace IonHeart {


struct ZoneDefinition {
    QString parentPath;
    QString name;
    Qt::Orientation orientation;
    QString after, before;
};

class IPanelWidget {
public:
    virtual QWidget *getWidget() = 0;
    virtual QString getPanelTitle() = 0;
    virtual QString getPanelZone() = 0;
};

}

#endif // PANELWIDGET_H
