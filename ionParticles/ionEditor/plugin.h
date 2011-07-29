#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>
#include <ionHeart/ionPulse/plugin.h>
#include <QScopedPointer>

namespace IonEditor {

class EditorWidgetFactory;

class Plugin : public QObject, public IonPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    void initialize(IonHeart::MainWindow *mainWindow);
    QString getName() {
        return "ionEditor";
    }

signals:

public slots:

};

}

#endif // IONEDITOR_H




