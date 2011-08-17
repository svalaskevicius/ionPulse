#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>
#include <ionHeart/plugin.h>
#include <QScopedPointer>

namespace IonEditor {

class EditorWidgetFactory;

class Plugin : public QObject, public IonHeart::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonHeart::IPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    ~Plugin();
    void initialize();
    QString getName() {
        return "ionEditor";
    }
    EditorWidgetFactory *getEditorWidgetFactory();
    virtual QList<IonHeart::IPanelWidget *> getPanelWidgets();

private:
    QScopedPointer<EditorWidgetFactory> _editorWidgetFactory;

signals:

public slots:
private:
    Q_DISABLE_COPY(Plugin)
};

}

#endif // IONEDITOR_H




