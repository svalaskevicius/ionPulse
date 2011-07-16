#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>
#include <ionPulse/plugin.h>

namespace IonEditor {


class Plugin : public QObject, public IonPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    void initialize(IonHeart::MainWindow *mainWindow);

signals:

public slots:

};

}

#endif // IONEDITOR_H
