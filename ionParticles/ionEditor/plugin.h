/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONEDITOR_H
#define IONEDITOR_H

#include <QObject>

#include <QScopedPointer>
#include "editorapi.h"
#include <ionParticles/ionLayout/layoutapi.h>

namespace IonEditor {

namespace Private {
class EditorWidgetBuilderImpl;

class Plugin : public QObject, public EditorPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonCore::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    ~Plugin();
    void postLoad();
    QString getName() {
        return EditorPlugin::name();
    }
    virtual QList<QString> getDependencies() {
        QList<QString> ret;
        ret.append(IonLayout::LayoutPlugin::name());
        return ret;
    }
    virtual void addParent(BasicPlugin *parent);
    EditorWidgetBuilder *getEditorWidgetBuilder();
private:
    QScopedPointer<Private::EditorWidgetBuilderImpl> _editorWidgetFactory;
    IonLayout::LayoutManager *layoutManager;
    QMap<QString, Editor *> openedFiles;
    Editor *focusedEditor;
signals:

public slots:
    void openFile(QString path, int line);
    void closeFileEditor(Editor *editor);
    void focusFileEditor(Editor *editor);
    void onFileSave();
    void onFileClose();
private:
    Q_DISABLE_COPY(Plugin)
};

}
}

#endif // IONEDITOR_H




