#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>

#include <QScopedPointer>
#include "editorapi.h"


namespace IonEditor {
namespace Private {
class EditorWidgetFactory;
}


class Plugin : public QObject, public IEditorPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonHeart::IPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    ~Plugin();
    void initialize();
    QString getName() {
        return EDITOR_PLUGIN_NAME;
    }
    IEditorWidgetFactory *getEditorWidgetFactory();
private:
    QScopedPointer<Private::EditorWidgetFactory> _editorWidgetFactory;

signals:

public slots:
    void openFile(QString path);

private:
    Q_DISABLE_COPY(Plugin)
};

}

#endif // IONEDITOR_H




