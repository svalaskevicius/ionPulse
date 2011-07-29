#ifndef INTERFACES_H
#define INTERFACES_H

#include "../mainwindow.h"

class IonPlugin {
public:
    virtual ~IonPlugin() {}
    virtual void initialize(IonHeart::MainWindow *mainWindow) = 0;
    virtual QString getName() = 0;
    virtual QList<QString> getDependencies() {
        return QList<QString>();
    }
    virtual void addParent(IonPlugin * /* parent */) {}
};

Q_DECLARE_INTERFACE(IonPlugin, "com.ionPulse.PluginInterface")

#endif // INTERFACES_H
