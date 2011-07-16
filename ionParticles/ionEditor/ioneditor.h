#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>
#include <ionPlugin.h>

class IonEditor : public QObject, public IonPlugin
{
    Q_OBJECT   
    Q_INTERFACES(IonPlugin)
public:
    explicit IonEditor(QObject *parent = 0);
    void initialize(IonHeartWindow *mainWindow);

signals:

public slots:

};

#endif // IONEDITOR_H
