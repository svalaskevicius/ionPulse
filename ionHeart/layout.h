#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>

namespace IonHeart {


struct ZoneDefinition {
    QString parentPath;
    QString name;
    Qt::Orientation orientation;
    QString after, before;
    bool hideIfEmpty;
    int sizeWeight;
};

class PanelWidget {
public:
    virtual QWidget *getWidget() = 0;
    virtual QString getPanelTitle() = 0;
    virtual QString getPanelZone() = 0;
};


class LayoutManager
{
public:
    virtual void add(PanelWidget *panel) = 0;
    virtual void addZone(ZoneDefinition &zone) = 0;
};


}

#endif // PANELWIDGET_H
