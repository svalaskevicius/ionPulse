#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>

#include <QScopedPointer>
#include "editorapi.h"


namespace IonEditor {
namespace Private {
class EditorWidgetBuilderImpl;
}


class Plugin : public QObject, public EditorPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonHeart::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    ~Plugin();
    void initialize();
    QString getName() {
        return EDITOR_PLUGIN_NAME;
    }
    EditorWidgetBuilder *getEditorWidgetBuilder();
private:
    QScopedPointer<Private::EditorWidgetBuilderImpl> _editorWidgetFactory;

signals:

public slots:
    void openFile(QString path);

private:
    Q_DISABLE_COPY(Plugin)
};

}

#endif // IONEDITOR_H




