#ifndef IONEDITORPROJECT_H
#define IONEDITORPROJECT_H

#include <QObject>
#include <ionHeart/plugin.h>

namespace IonProject {


class Plugin : public QObject, public IonHeart::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonHeart::IPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    void initialize();
    QString getName() {
        return "ionProject";
    }
    virtual QList<QString> getDependencies() {
        QList<QString> ret;
        ret.append("ionEditor");
        return ret;
    }
    virtual void addParent(IPlugin *parent);

signals:

public slots:

};

}

#endif // IONEDITORPROJECT_H




