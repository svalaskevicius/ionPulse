#ifndef INTERFACES_H
#define INTERFACES_H

#include "mainwindow.h"
#include "layout.h"
#include <QList>

namespace IonHeart {

class IPlugin {
protected:
    ILayoutManager *layoutManager;
public:
    void setLayoutManager(ILayoutManager *layoutManager) {this->layoutManager = layoutManager;}
    virtual void initialize() = 0;
    virtual QString getName() = 0;
    virtual QList<QString> getDependencies() {
        return QList<QString>();
    }
    virtual void addParent(IPlugin * /* parent */) {}
};

}

Q_DECLARE_INTERFACE(IonHeart::IPlugin, "com.ionPulse.PluginInterface")

#endif // INTERFACES_H
