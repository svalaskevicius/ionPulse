/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

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
