/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>
#include <ionCore/plugin.h>

namespace IonLayout {

struct ZoneDefinition
{
    QString parentPath;
    QString name;
    Qt::Orientation orientation;
    QString after, before;
    bool hideIfEmpty;
    int sizeWeight;
    bool childrenClosable;
};

class PanelWidget
{
public:
    virtual QWidget *getWidget() = 0;
    virtual QString getPanelTitle() = 0;
    virtual QString getPanelZone() = 0;
};


class LayoutManager : public QObject
{
    Q_OBJECT
public:
    virtual ~LayoutManager() {}
    virtual void add(PanelWidget *panel) = 0;
    virtual void remove(PanelWidget *panel) = 0;
    virtual void focus(PanelWidget *panel) = 0;
    virtual void addZone(ZoneDefinition &zone) = 0;
public slots:
    virtual void updatePanelTitle(PanelWidget *panel) = 0;
};


class LayoutPlugin : public IonCore::BasicPlugin
{
public:
    virtual LayoutManager *getLayoutManager() = 0;
    static QString name() { return "ionLayout"; }
};

}

#endif // PANELWIDGET_H
