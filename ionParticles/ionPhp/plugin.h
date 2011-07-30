#ifndef IONEDITORPHP_H
#define IONEDITORPHP_H

#include <QObject>
#include <ionHeart/plugin.h>

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

#endif // IONEDITORPHP_H




