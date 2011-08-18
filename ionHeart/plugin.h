#ifndef INTERFACES_H
#define INTERFACES_H

#include "mainwindow.h"
#include "panelwidget.h"
#include <QList>

namespace IonHeart {

class IPlugin {
public:
    virtual void initialize() = 0;
    virtual QString getName() = 0;
    virtual QList<QString> getDependencies() {
        return QList<QString>();
    }
    virtual void addParent(IPlugin * /* parent */) {}
    virtual QList<IPanelWidget *> getPanelWidgets() {return QList<IPanelWidget *>();}
    virtual QList<ZoneDefinition> getZoneDefinitions() {return QList<ZoneDefinition>();}
};

}

Q_DECLARE_INTERFACE(IonHeart::IPlugin, "com.ionPulse.PluginInterface")

#endif // INTERFACES_H
