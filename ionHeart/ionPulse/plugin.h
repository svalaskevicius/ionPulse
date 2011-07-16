#ifndef INTERFACES_H
#define INTERFACES_H

#include "mainwindow.h"

class IonPlugin {
public:
    virtual ~IonPlugin() {}
    virtual void initialize(IonHeart::MainWindow *mainWindow) = 0;
};

Q_DECLARE_INTERFACE(IonPlugin, "com.ionPulse.PluginInterface")

#endif // INTERFACES_H
