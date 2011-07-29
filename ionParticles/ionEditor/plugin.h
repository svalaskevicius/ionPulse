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
    ~Plugin();
    void initialize(IonHeart::MainWindow *mainWindow);
    QString getName() {
        return "ionEditor";
    }
    EditorWidgetFactory *getEditorWidgetFactory();

private:
    QScopedPointer<EditorWidgetFactory> _editorWidgetFactory;

signals:

public slots:
private:
    Q_DISABLE_COPY(Plugin)
};

}

#endif // IONEDITOR_H




