#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>
#include <ionPulse/plugin.h>

namespace IonPhp {


class Plugin : public QObject, public IonPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    void initialize(IonHeart::MainWindow * /* mainWindow */);
    QString getName() {
        return "ionPhp";
    }
    virtual QList<QString> getDependencies() {
        QList<QString> ret;
        ret.append("ionEditor");
        return ret;
    }
    virtual void addParent(IonPlugin *parent);

signals:

public slots:

};

}

#endif // IONEDITOR_H




